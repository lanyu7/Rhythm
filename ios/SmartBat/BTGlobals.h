//
//  BTConfigs.h
//  SmartBat
//
//  Created by kaka' on 13-6-4.
//  Copyright (c) 2013年 kaka'. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "BTAppDelegate.h"
#import "BTEntity.h"
#import "BTConstants.h"

@interface BTGlobals : NSObject{
    NSManagedObjectContext* _context;
    BTEntity* _globalsInEntity;
}

//这里是全局变量
@property(assign, nonatomic) int beatPerMinute;
@property(assign, nonatomic) int beatPerMeasure;
@property(assign, nonatomic) float noteType;
@property(assign, nonatomic) int subdivision;


@property(assign, nonatomic) int lastCheckVersionDate;
@property(assign, nonatomic) int installDate;
@property(assign, nonatomic) int hasAskGrade;

@property(assign, nonatomic) double currentMeasureDuration;
@property(assign, nonatomic) double currentNoteDuration;
@property(assign, nonatomic) double currentSubdivisionDuration;
@property(strong, nonatomic) NSArray * currentMeasure;


//indexOfMeasure: 0
//hitTime: mach_absolute_time
//type: BeatType: 100, 101
@property(retain, nonatomic) NSDictionary * beatInfo;

//playStatus : YES/NO, Play Status
//playStatusChangedTime : mach_absolute_time, Play Status changed time
@property(retain, nonatomic) NSDictionary * systemStatus;

//手环等着下一个重拍到达
@property(assign, nonatomic) Boolean waitForRestart;

//手环总数和数组
@property(assign, nonatomic) NSInteger bleListCount;
@property(strong, nonatomic) NSMutableArray* bleList;

//手机是否连接设备
@property(assign, nonatomic) Boolean isConnectedBLE;

//手环状态
@property(assign, nonatomic) Boolean bleShock;
@property(assign, nonatomic) Boolean bleSpark;

+(BTGlobals*)sharedGlobals;
-(void)applicationWillResignActive:(NSNotification*) notification;

@end
