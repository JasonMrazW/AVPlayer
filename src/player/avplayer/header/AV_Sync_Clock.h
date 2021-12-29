//
// Created by bo on 2021/12/28.
//

#ifndef AVPLAYER_AV_SYNC_CLOCK_H
#define AVPLAYER_AV_SYNC_CLOCK_H
extern "C" {
#include "libavutil/time.h"
#include "libavutil/common.h"
};
typedef struct Clock {
    double pts;           /* clock base */
    double pts_drift;     /* clock base minus time at which we updated the clock */
    double last_updated;
    double speed;
} Clock;
#define MicroSecondToSecondUnit 1000000.0

/* no AV sync correction is done if below the minimum AV sync threshold */
#define AV_SYNC_THRESHOLD_MIN 0.04
/* AV sync correction is done if above the maximum AV sync threshold */
#define AV_SYNC_THRESHOLD_MAX 0.1
/* If a frame duration is longer than this, it will not be duplicated to compensate AV sync */
#define AV_SYNC_FRAMEDUP_THRESHOLD 0.1
/* no AV correction is done if too big error */
#define AV_NOSYNC_THRESHOLD 10.0

class SyncClock {
public:
    /**
     * 初始化时钟
     * @param clock
     */
    static void initClock(Clock *clock) {
        clock->speed = 1.0;//正常播放速度
        setClockAtTime(clock, NAN, getCurrentRelativeTime());
    }

    /**
     * 更新clock时钟
     * @param c
     * @param pts
     * @param time
     */
    static void setClockAtTime(Clock *clock, double pts, double time) {
        clock->pts = pts;
        clock->last_updated = time;
        //计算下一帧显示时间，与当前的时间差
        clock->pts_drift = clock->pts - time;
    }

    /**
     * 获取Clock时钟内真实的pts，单位：秒
     * @param c
     * @return
     */
    static double getClockTime(Clock *clock) {
        double time = getCurrentRelativeTime();
        return clock->pts_drift + time - (time - clock->last_updated) * (1.0 - clock->speed);
    }

    /**
     * 同步音频时钟
     * @param from
     * @param to
     */
    static void syncClock(Clock *from, Clock *to) {
        double from_clock_time = getClockTime(from);
        double to_clock_time = getClockTime(to);

        if (!isnan(from_clock_time) && (isnan(to_clock_time) || fabs(from_clock_time-to_clock_time) > AV_NOSYNC_THRESHOLD)) {
            setClockAtTime(to, from_clock_time, getCurrentRelativeTime());
        }
    }

    /**
     * 获取当前相对时间戳
     * @return
     */
    static double getCurrentRelativeTime() {
        return av_gettime_relative()/MicroSecondToSecondUnit;
    }

    /**
     * 矫正视频的延迟时间
     * @param video_clock
     * @param audio_clock
     * @param video_pts
     * @return
     */
    static double computeVideoFrameDelay(Clock *video_clock, Clock *audio_clock, double delay) {
        double av_diff = getClockTime(video_clock) - getClockTime(audio_clock);
        //保证delay的阈值是在AV_SYNC_THRESHOLD_MIN~AV_SYNC_THRESHOLD_MAX之间
        double sync_threshold = FFMAX(AV_SYNC_THRESHOLD_MIN, FFMIN(AV_SYNC_THRESHOLD_MAX, delay));

        if (av_diff <= -sync_threshold) {//视频落后于音频
            delay = FFMAX(0, delay + av_diff);
        } else if(av_diff >= sync_threshold && delay > AV_SYNC_FRAMEDUP_THRESHOLD) {//视频超前太多了，delay一步到位
            delay += av_diff;
        } else if (av_diff >= sync_threshold) {
            delay *= 2;
        }

        return delay;
    }
};
#endif //AVPLAYER_AV_SYNC_CLOCK_H
