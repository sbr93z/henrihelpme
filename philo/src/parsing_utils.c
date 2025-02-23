/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sferrad <sferrad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 15:30:24 by sferrad           #+#    #+#             */
/*   Updated: 2025/02/11 20:11:24 by sferrad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

int	ft_atoi(const char *nptr)
{
	int	i;
	int	result;
	int	signe;

	signe = 1;
	result = 0;
	i = 0;
	while (nptr[i] == ' ' || (nptr[i] >= 9 && nptr[i] <= 13))
		i++;
	if (nptr[i] == '-' || nptr[i] == '+')
	{
		if (nptr[i] == '-')
			signe = signe * -1;
		i++;
	}
	while (nptr[i] && nptr[i] >= '0' && nptr[i] <= '9')
	{
		result = result * 10 + (nptr[i] - '0');
		i++;
	}
	return (result * signe);
}

int	is_digit_str(const char *str)
{
	int	i;

	i = 0;
	if (str[i] == '-' || str[i] == '+')
		i++;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

int	parsing(t_data data, int ac, char **av)
{
	int	i;

	if (ac >= 5)
	{
		if (ac > 6)
			return (0);
		i = 1;
		while (i < ac)
		{
			if (!is_digit_str(av[i]))
				return (0);
			i++;
		}
		if (data.number_of_philo <= 0 || data.time_to_die < 0
			|| data.time_to_eat < 0 || data.time_to_sleep < 0 || (ac == 6
				&& data.number_of_eat <= 0))
			return (0);
		return (1);
	}
	return (0);
}
