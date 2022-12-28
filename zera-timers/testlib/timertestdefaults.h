#ifndef TIMERTESTDEFAULTS_H
#define TIMERTESTDEFAULTS_H

// These should remain fixed (huge) because
// * here we are testing delays and timers span widely in oe-ctest
// * when using SingleShotTimerTest there are no delays
static constexpr int DEFAULT_EXPIRE = 50;
static constexpr int DEFAULT_EXPIRE_WAIT = DEFAULT_EXPIRE * 2;
static constexpr int EXPIRE_INFINITE = 1000000;

#endif // TIMERTESTDEFAULTS_H
