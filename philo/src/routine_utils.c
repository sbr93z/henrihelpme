/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sferrad <sferrad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 21:20:19 by sferrad           #+#    #+#             */
/*   Updated: 2025/02/22 19:29:14 by sferrad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

void	take_forks(t_data *data, t_philo *philo)
{
	if (!philo || !data)
		return ;
	if (philo->id % 2)
	{
		pthread_mutex_lock(&philo->right_fork->fork);
		pthread_mutex_lock(&data->end_simulation_mutex); // Lock here
		if (data->end_simulation)
		{
			pthread_mutex_unlock(&data->end_simulation_mutex); // Unlock before return
			pthread_mutex_unlock(&philo->right_fork->fork);
			return ;
		}
		print_status(data, philo->id, "has taken a fork");
		pthread_mutex_unlock(&data->end_simulation_mutex); // Unlock here

		pthread_mutex_lock(&philo->left_fork->fork);
		pthread_mutex_lock(&data->end_simulation_mutex); // Lock here
		if (data->end_simulation)
		{
			pthread_mutex_unlock(&data->end_simulation_mutex); // Unlock before return
			pthread_mutex_unlock(&philo->left_fork->fork);
			pthread_mutex_unlock(&philo->right_fork->fork);
			return ;
		}
		print_status(data, philo->id, "has taken a fork");
		pthread_mutex_unlock(&data->end_simulation_mutex); // Unlock here
	}
	else
	{
		pthread_mutex_lock(&philo->left_fork->fork);
		pthread_mutex_lock(&data->end_simulation_mutex); // Lock here
		if (data->end_simulation)
		{
			pthread_mutex_unlock(&data->end_simulation_mutex); // Unlock before return
			pthread_mutex_unlock(&philo->left_fork->fork);
			return ;
		}
		print_status(data, philo->id, "has taken a fork");
		pthread_mutex_unlock(&data->end_simulation_mutex); // Unlock here

		pthread_mutex_lock(&philo->right_fork->fork);
		pthread_mutex_lock(&data->end_simulation_mutex); // Lock here
		if (data->end_simulation)
		{
			pthread_mutex_unlock(&data->end_simulation_mutex); // Unlock before return
			pthread_mutex_unlock(&philo->right_fork->fork);
			pthread_mutex_unlock(&philo->left_fork->fork);
			return ;
		}
		print_status(data, philo->id, "has taken a fork");
		pthread_mutex_unlock(&data->end_simulation_mutex); // Unlock here
	}
}

void	start_eating(t_data *data, t_philo *philo)
{
	if (!philo || !data)
		return ;

	pthread_mutex_lock(&philo->last_meal_time_mutex);
	philo->last_meal_time = get_timestamp();
	pthread_mutex_unlock(&philo->last_meal_time_mutex);

	pthread_mutex_lock(&data->end_simulation_mutex);
	if (data->end_simulation)
	{
		pthread_mutex_unlock(&data->end_simulation_mutex); // Unlock before return
		pthread_mutex_unlock(&philo->right_fork->fork);
		pthread_mutex_unlock(&philo->left_fork->fork);
		return ;
	}
	pthread_mutex_unlock(&data->end_simulation_mutex); // Unlock here

	print_status(data, philo->id, "is eating");
	usleep(data->time_to_eat * 1000);
	pthread_mutex_unlock(&philo->left_fork->fork);
	pthread_mutex_unlock(&philo->right_fork->fork);
}

void check_death_conditions(t_philo *philo, long time_since_last_meal)
{
    pthread_mutex_lock(&philo->data->end_simulation_mutex); // Locking here
    if (time_since_last_meal > philo->data->time_to_die)
    {
        if (!philo->data->end_simulation)
        {
            philo->data->end_simulation = 1;
            print_status(philo->data, philo->id, "died");
        }
    }
    pthread_mutex_unlock(&philo->data->end_simulation_mutex); // Unlocking here
}



bool	should_continue_eating(t_philo *philo)
{
	t_data *data;
	bool continue_eating;

	data = philo->data;
	pthread_mutex_lock(&philo->meal_count_mutex);
	if (!philo->is_full && philo->meals_count < data->number_of_eat)
		continue_eating = true;
	else
		continue_eating = false;
	pthread_mutex_unlock(&philo->meal_count_mutex);
	return (continue_eating);
}