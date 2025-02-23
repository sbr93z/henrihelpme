/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sferrad <sferrad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/11 17:54:47 by sferrad           #+#    #+#             */
/*   Updated: 2025/02/22 23:15:49 by sferrad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

long	get_timestamp(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000 + tv.tv_usec / 1000));
}

void eat(t_data *data, t_philo *philo)
{
    if (!philo || !data)
        return ;
    take_forks(data, philo);
    if (!data->end_simulation)
        start_eating(data, philo);

    pthread_mutex_lock(&philo->meal_count_mutex);
    philo->meals_count++;
    
    if (philo->data->number_of_eat != -1 
        && philo->meals_count >= philo->data->number_of_eat) 
    {
        philo->is_full = true;
    }
    
    // Vérifier si c'est le dernier repas
    pthread_mutex_lock(&data->last_meal_mutex);
    if (!data->last_meal_finished) 
    {
        data->last_meal_finished = true;
        print_status(data, philo->id, "is eating");
    }
    pthread_mutex_unlock(&data->last_meal_mutex);
    
    pthread_mutex_unlock(&philo->meal_count_mutex);

    pthread_mutex_lock(&philo->last_meal_time_mutex);
    philo->last_meal_time = get_timestamp();
    pthread_mutex_unlock(&philo->last_meal_time_mutex);
}

void	ft_sleep(t_data *data, t_philo *philo)
{
	long	start_time;

	bool should_continue ;
	pthread_mutex_lock(&data->end_simulation_mutex);
	should_continue = !data->end_simulation;
	pthread_mutex_unlock(&data->end_simulation_mutex);
	if (!should_continue)
		return ;
	print_status(data, philo->id, "is sleeping");
	start_time = get_timestamp();
	while ((get_timestamp() - start_time) < data->time_to_sleep)
	{
		pthread_mutex_lock(&data->end_simulation_mutex);
		should_continue = !data->end_simulation;
		pthread_mutex_unlock(&data->end_simulation_mutex);
		if (!should_continue)
			return ;
		usleep(1000);
	}
	pthread_mutex_lock(&data->end_simulation_mutex);
	should_continue = !data->end_simulation;
	pthread_mutex_unlock(&data->end_simulation_mutex);
	if (!should_continue)
		return ;
	print_status(data, philo->id, "is thinking");
}

void	*routine(void *arg)
{
	t_philo	*philo;

	// bool should_continue ;
	philo = (t_philo *)arg;
	if (philo->id % 2)
		usleep(100);
	while (1)
	{
		pthread_mutex_lock(&philo->data->end_simulation_mutex);
		// Lock before checking
		if (philo->data->end_simulation)
		{
			pthread_mutex_unlock(&philo->data->end_simulation_mutex);
			// Unlock after
			break ;
		}
		pthread_mutex_unlock(&philo->data->end_simulation_mutex);
		// Unlock before next loop iteration
		if (philo->data->number_of_eat != -1)
			if (!should_continue_eating(philo))
				break ;
		eat(philo->data, philo);
		// if (philo->is_full)
		// 	break ;
		ft_sleep(philo->data, philo);
	}
	return (NULL);
}

void *check_death(void *arg)
{
    t_philo *philo;
    long time_since_last_meal;
    bool should_continue;

    philo = (t_philo *)arg;
    while (1)
    {
        pthread_mutex_lock(&philo->last_meal_time_mutex);
        time_since_last_meal = get_timestamp() - philo->last_meal_time;
        pthread_mutex_unlock(&philo->last_meal_time_mutex);
        check_death_conditions(philo, time_since_last_meal);

        // Vérifier si la simulation doit s'arrêter
        pthread_mutex_lock(&philo->data->end_simulation_mutex);
        should_continue = !philo->data->end_simulation;
        pthread_mutex_unlock(&philo->data->end_simulation_mutex);
        
        pthread_mutex_lock(&philo->data->last_meal_mutex);
        if (philo->data->last_meal_finished) 
        {
            pthread_mutex_unlock(&philo->data->last_meal_mutex);
            break ;
        }
        pthread_mutex_unlock(&philo->data->last_meal_mutex);
        
        if (!should_continue)
            break ;
        usleep(1000);
    }
    return (NULL);
}
