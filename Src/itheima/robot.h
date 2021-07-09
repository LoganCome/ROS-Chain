//
// Created by Kaijun on 2021/1/29.
//

#ifndef HEIMAROBOTV6_ROBOT_H
#define HEIMAROBOTV6_ROBOT_H

# ifdef __cplusplus
    extern "C" {
#endif
/**
 *  相当于是一个桥梁, C 能够 调用CPP
 */

/**
 * 机器人初始化
 */
void HeimaRobotInit();
/**
 * 机器人心跳: 永真循环
 */
void HeimaRobotTick();


#ifdef __cplusplus
};
#endif


#endif //HEIMAROBOTV6_ROBOT_H
