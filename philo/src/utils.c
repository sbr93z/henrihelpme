/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sferrad <sferrad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/11 15:39:45 by sferrad           #+#    #+#             */
/*   Updated: 2025/02/23 20:34:12 by sferrad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

int	init_data(t_data *data, int ac, char **av)
{
	data->number_of_philo = ft_atoi(av[1]);
	data->time_to_die = ft_atoi(av[2]);
	data->time_to_eat = ft_atoi(av[3]);
	data->time_to_sleep = ft_atoi(av[4]);
	if (ac == 6)
		data->number_of_eat = ft_atoi(av[5]);
	else
		data->number_of_eat = -1;
	data->end_simulation = 0;
	data->last_meal_finished = false; // Initialisation
	// Initialiser les mutex avant d'allouer la mémoire
	if (pthread_mutex_init(&data->end_simulation_mutex, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&data->print_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&data->end_simulation_mutex);
		return (0);
	}
	if (pthread_mutex_init(&data->last_meal_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&data->end_simulation_mutex);
		pthread_mutex_destroy(&data->print_mutex);
		return (0);
	}
	init_philosophers_and_forks(data);
	if (!data->forks || !data->philos)
	{
		pthread_mutex_destroy(&data->end_simulation_mutex);
		pthread_mutex_destroy(&data->print_mutex);
		pthread_mutex_destroy(&data->last_meal_mutex);
		return (0);
	}
	return (1);
}

void	init_fork(t_fork *fork, int id)
{
	fork->fork_id = id;
	if (pthread_mutex_init(&fork->fork, NULL) != 0)
		return ; // Gérer l'erreur
}

void	init_philosopher(t_data *data, int id)
{
	t_philo	*philo;

	philo = &data->philos[id];
	philo->id = id + 1;
	philo->meals_count = 0;
	philo->last_meal_time = get_timestamp();
	philo->left_fork = &data->forks[id];
	philo->right_fork = &data->forks[(id + 1) % data->number_of_philo];
	philo->data = data;
	philo->is_full = false; // Ajouter cette ligne
	if (pthread_mutex_init(&philo->meal_count_mutex, NULL) != 0)
		return ; // Gérer l'erreur
	if (pthread_mutex_init(&philo->last_meal_time_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&philo->meal_count_mutex);
		return ; // Gérer l'erreur
	}
	if (data->number_of_eat != -1)
	{
		if (pthread_mutex_init(&philo->is_full_mutex, NULL) != 0)
		{
			pthread_mutex_destroy(&philo->meal_count_mutex);
			pthread_mutex_destroy(&philo->last_meal_time_mutex);
			return ; // Gérer l'erreur
		}
	}
}

void	init_philosophers_and_forks(t_data *data)
{
	int	i;

	data->forks = malloc(sizeof(t_fork) * data->number_of_philo);
	if (!data->forks)
		return ;
	data->philos = malloc(sizeof(t_philo) * data->number_of_philo);
	if (!data->philos)
	{
		free(data->forks);
		return ;
	}
	i = -1;
	while (++i < data->number_of_philo)
	{
		init_fork(&data->forks[i], i);
		init_philosopher(data, i);
	}
}
void	ft_free_ressources(t_data *data)
{
	int	i;

	if (data->philos)
	{
		i = 0;
		while (i < data->number_of_philo)
		{
			pthread_mutex_destroy(&data->philos[i].meal_count_mutex);
			pthread_mutex_destroy(&data->philos[i].last_meal_time_mutex);
			if (data->number_of_eat != -1)
			{
				pthread_mutex_destroy(&data->philos[i].is_full_mutex);
			}
			i++;
		}
		free(data->philos);
	}
	if (data->forks)
	{
		i = 0;
		while (i < data->number_of_philo)
		{
			pthread_mutex_destroy(&data->forks[i].fork);
			i++;
		}
		free(data->forks);
	}
	pthread_mutex_destroy(&data->end_simulation_mutex);
	pthread_mutex_destroy(&data->print_mutex);
	pthread_mutex_destroy(&data->last_meal_mutex);
	// Vérification avant destruction du mutex is_full_mutex
// 	if (data->number_of_eat != -1)
// 	{
// 		pthread_mutex_destroy(&data->is_full_mutex);
// 	}
}

void	print_status(t_data *data, int philo_id, char *message)
{
	pthread_mutex_lock(&data->print_mutex);
	if (!data->end_simulation)
		printf("%ld %d %s\n", get_timestamp() - data->start_simulation,
			philo_id, message);
	pthread_mutex_unlock(&data->print_mutex);
}
