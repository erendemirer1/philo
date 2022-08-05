/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   grim_reaper.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcombeau <mcombeau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/04 12:00:18 by mcombeau          #+#    #+#             */
/*   Updated: 2022/08/05 17:23:01 by mcombeau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

/* kill_all_philos:
*	Sends the kill signal to all philosopher processes. Used to send the
*	simulation if a philosopher has died or if a philosopher process has
*	encountered a fatal error.
*	Returns the given exit code.
*/
int	kill_all_philos(t_table *table, int exit_code)
{
	unsigned int	i;

	i = 0;
	while (i < table->nb_philos)
	{
		kill(table->pids[i], SIGKILL);
		i++;
	}
	return (exit_code);
}

/* end_condition_reached:
*	Checks this philosopher to see if one of two end conditions
*	has been reached. Exits the process with specific exit codes
*	if the philosopher has died or has eaten enough.
*	Returns false if the philosopher is alive and well.
*/
static bool	end_condition_reached(t_table *table, t_philo *philo)
{
	sem_wait(philo->sem_meal);
	if (get_time_in_ms() - philo->last_meal >= table->time_to_die)
	{
		write_status(philo, true, DIED);
		child_exit(table, CHILD_EXIT_PHILO_DEAD);
	}
	if (table->must_eat_count != -1
		&& philo->times_ate >= (unsigned int)table->must_eat_count)
	{
		while (philo->nb_forks_held != 0)
		{
			sem_post(philo->sem_forks);
			philo->nb_forks_held -= 1;
		}
		child_exit(table, CHILD_EXIT_PHILO_FULL);
	}
	sem_post(philo->sem_meal);
	return (false);
}

/* personal_grim_reaper:
*	The grim reaper thread's routine. Checks if this philosopher must
*	be killed and if he ate enough. If one of those two
*	end conditions are reached, terminate the philosopher process
*	with the appropriate exit code.
*/
void	*personal_grim_reaper(void *data)
{
	t_table			*table;

	table = (t_table *)data;
	sim_start_delay(table->start_time);
	if (table->must_eat_count == 0)
		child_exit(table, CHILD_EXIT_PHILO_FULL);
	while (!end_condition_reached(table, table->this_philo))
		usleep(5000);
	return (NULL);
}
