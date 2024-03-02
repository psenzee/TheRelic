//
//  SoundsListView.h
//
//  Created by Paul Senzee on 8/30/14.
//  Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "EasyTableView.h"

@class SimpleCall;

@interface SoundsListView : UIView <EasyTableViewDelegate>
{
    UIButton        *done;
    NSMutableArray  *buttons;
}

- (id)initWithFrame:(CGRect)frame;
- (void)update;

// EasyTableViewDelegate
- (UIView *)easyTableView:(EasyTableView *)easyTableView viewForRect:(CGRect)rect;
- (void)easyTableView:(EasyTableView *)easyTableView setDataForView:(UIView *)view forIndex:(NSUInteger)index;

@end
