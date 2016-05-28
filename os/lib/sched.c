#include "env.h"
#include "gpio.h"

/* Overview:
 *  Implement simple round-robin scheduling.
 *  Search through 'envs' for a runnable environment ,
 *  in circular fashion statrting after the previously running env,
 *  and switch to the first such environment found.
 *
 * Hints:
 *  The variable which is for counting should be defined as 'static'.
 */
void sched_yield(void)
{
    static int index = 0;
    extern struct Env *envs;
    while (1)
    {
        for (++index; index < NENV; ++index)
        {
            struct Env *thisEnv = &envs[index];
            if (thisEnv->env_status == ENV_RUNNABLE)
            {
                if (index % 2)
                {
                    gpio_clr(17);
                    gpio_set(27);
                }
                else
                {
                    gpio_set(17);
                    gpio_clr(27);
                }
                env_run(thisEnv);
                return;
            }
        }
        index = -1;
    }
}
