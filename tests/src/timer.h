#ifndef ASTOLFO_TIMER_H
#define ASTOLFO_TIMER_H

#include <sys/time.h>
#include <unistd.h>


class Timer {

    private:
        struct timeval tp1;

    public:
        Timer(){
            reset();
        }
        void reset(){
            gettimeofday(&tp1, 0);
        }
        unsigned long get_us(){
            struct timeval tp3;
            gettimeofday(&tp3, 0);
            return tp3.tv_usec - tp1.tv_usec + (tp3.tv_sec - tp1.tv_sec) * 1000000;
        }
        unsigned long get_us_reset(){
            unsigned long ret = get_us();
            reset();
            return ret;
        }
        void rate_limit(int hz){
            int targ = 1000000 / hz - get_us_reset();
            if(targ > 0){
                usleep(targ);
            }
            return;
        }
};

#endif