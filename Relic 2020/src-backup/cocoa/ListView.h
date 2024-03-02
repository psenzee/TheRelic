//
//  ListView.h
//
//  Created by Paul Senzee on 8/30/14.
//  Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "EasyTableView.h"

@class SimpleCall;

@protocol ListDelegate <NSObject>

- (int)       listCount;
- (void)      listUpdate;
- (NSString *)listNameAtIndex:(int)index;
- (void)      listSelectIndex:(int)index;
- (id)        listObjectAtIndex:(int)index;

@end

@interface ListView : UIView <EasyTableViewDelegate>
{
    UIButton      *done;
    UIButton      *del;
    UIButton      *test;
    UILabel       *title;
    EasyTableView *table;
    UIColor       *cellColorOdd;
    UIColor       *cellColorEven;
    UIColor       *cellHighlightColor;
}

@property (nonatomic, retain) id<ListDelegate>  delegate;

@property (nonatomic, retain) SimpleCall       *onAdd;
@property (nonatomic, retain) SimpleCall       *onDone;
@property (nonatomic, retain) SimpleCall       *onDelete;
@property (nonatomic, retain) SimpleCall       *onTest;
@property (nonatomic, retain) SimpleCall       *onSelected;
@property (nonatomic, retain) id                selectedObject;
@property (nonatomic)         int               selected;

- (id)initWithFrame:(CGRect)frame withTitle:(NSString *)title;
- (void)setTitle:(NSString *)title;
- (void)setCellOddColor:(UIColor *)oddColor evenColor:(UIColor *)evenColor highlightColor:(UIColor *)hlcolor;

- (void)deleteEnabled:(BOOL)enabled;
- (void)update;

// EasyTableViewDelegate
- (UIView *)  easyTableView:(EasyTableView *)easyTableView viewForRect:(CGRect)rect;
- (void)      easyTableView:(EasyTableView *)easyTableView setDataForView:(UIView *)view forIndex:(NSUInteger)index;
- (NSUInteger)numberOfCellsForEasyTableView:(EasyTableView *)view;
- (BOOL)      canUserSelectIndexPath:(NSIndexPath *)indexPath;

- (void)easyTableView:(EasyTableView *)easyTableView
         selectedView:(UIView *)selView atIndex:(NSUInteger)selIndex
       deselectedView:(UIView *)desView deselectedIndex:(NSUInteger)desIndex;

@end
