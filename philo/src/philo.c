/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sferrad <sferrad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 12:05:00 by sferrad           #+#    #+#             */
/*   Updated: 2025/02/23 18:08:19 by sferrad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

int	create_threads(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->number_of_philo)
	{
		if (pthread_create(&data->philos[i].thread_id, NULL, routine,
				&data->philos[i]) != 0)
			return (0);
		if (pthread_create(&data->philos[i].death_thread_id, NULL, check_death,
				&data->philos[i]) != 0)
			return (0);
		i++;
	}
	i = 0;
	while (i < data->number_of_philo)
	{
		if (pthread_join(data->philos[i].thread_id, NULL) != 0)
			return (0);
		if (pthread_join(data->philos[i].death_thread_id, NULL) != 0)
			return (0);
		i++;
	}
	return (1);
}

int	start_simulation(t_data *data)
{
	data->start_simulation = get_timestamp();
	// usleep(1000);
	data->end_simulation = 0;
	if (!create_threads(data))
		return (0);
	return (1);
}

int	main(int ac, char **av)
{
	t_data	data;

	if (ac < 5)
	{
		printf("ERROR: Not enough arguments\n");
		return (1);
	}
	if (!init_data(&data, ac, av))
	{
		printf("ERROR: Invalid arguments\n");
		return (1);
	}
	if (!parsing(data, ac, av) || !start_simulation(&data))
	{
		printf("ERROR: Invalid arguments or thread creation failed\n");
		ft_free_ressources(&data);
		return (1);
	}
	ft_free_ressources(&data);
	return (0);
}
