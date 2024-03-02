//
//  ListView.m
//
//  Created by Paul Senzee on 8/30/14.
//  Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#import "ListView.h"

#import <UIKit/UIKit.h>
#import "RectUtil.h"
#import "SimpleCall.h"
#import "UITheme.h"
#import "EasyTableView.h"
#import "UIUtil.h"
#import "SysUtil.h"

@implementation ListView

- (void)donePressed
{
    [SimpleCall call:self.onDone];
}

- (void)testPressed
{
    [SimpleCall call:self.onTest];
}

- (void)deletePressed
{
    [SimpleCall call:self.onDelete];
}

- (id)initWithFrame:(CGRect)frame withTitle:(NSString *)title_
{
    self = [super initWithFrame:frame];
    if (self)
    {
        title = [UITheme defaultTitleLabel:title_ withFrame:RectInsideTop(self.frame, RealHeight(70.0))];
        [self addSubview:title];
        title.backgroundColor = [UITheme defaultDarkBackgroundColor];
        
        Border sborder = MakeBorder(RealWidth(20.0), RealWidth(20.0), RealHeight(80.0), RealHeight(80.0));
        CGRect fr = ShrinkRectWithBorder(self.bounds, sborder);
        table = [[EasyTableView alloc] initWithFrame:fr numberOfRows:0 ofHeight:RealHeight(80.0)];
        table.backgroundColor = [UIColor clearColor];
        table.tableView.backgroundColor = [UIColor clearColor];
        table.delegate = self;
        
        [self setCellOddColor:[UIColor colorWithRed:0.50 green:0.0 blue:0.0 alpha:1.0]
                    evenColor:[UIColor colorWithRed:0.65 green:0.0 blue:0.0 alpha:1.0]
               highlightColor:[UIColor colorWithRed:1.00 green:0.0 blue:0.0 alpha:1.0]];
        
        [self addSubview:table];
        
        CGRect bottomfr = RectInsideBottom(self.frame, RealHeight(70.0));
        
        UILabel *darkbar = [[UILabel alloc] initWithFrame:bottomfr];
        darkbar.backgroundColor = [UITheme defaultDarkBackgroundColor];
        [self addSubview:darkbar];
        
        CGRect donefr = RectWithWidth(bottomfr, bottomfr.size.width /* / 3.0 */);
//        CGRect delfr  = RectWithX(donefr, donefr.size.width * 1.0);
//        CGRect testfr = RectWithX(donefr, donefr.size.width * 2.0);

        [self addSubview:(done = [UITheme defaultButton:@"Return to Main Menu" withFrame:donefr])];
//        [self addSubview:(del  = [UITheme defaultButton:@"Delete" withFrame:delfr])];
//        [self addSubview:(test = [UITheme defaultButton:@"Test" withFrame:testfr])];
        
        done.backgroundColor /*= del.backgroundColor = test.backgroundColor*/ = [UIColor clearColor];
        
        [done addTarget:self action:@selector(donePressed) forControlEvents:UIControlEventTouchUpInside];
 //       [del addTarget:self action:@selector(deletePressed) forControlEvents:UIControlEventTouchUpInside];
 //       [test addTarget:self action:@selector(testPressed) forControlEvents:UIControlEventTouchUpInside];
        
        ListenForPropertyChange_Hidden(self, self, @selector(onHiddenChange));        
    }
    return self;
}

- (void)setTitle:(NSString *)str
{
    title.text = str;
}

- (void)deleteEnabled:(BOOL)enabled
{
    del.hidden = !enabled;
}

- (void)setCellOddColor:(UIColor *)oddColor evenColor:(UIColor *)evenColor highlightColor:(UIColor *)hlcolor
{
    [oddColor retain]; [evenColor retain]; [hlcolor retain];
    cellColorOdd = oddColor;
    cellColorEven = evenColor;
    cellHighlightColor = hlcolor;
}

- (UIView *)easyTableView:(EasyTableView *)easyTableView viewForRect:(CGRect)rect
{
    Border sb = MakeBorder(1.0, 3.0, 1.0, 3.0);

    UILabel *label = [UITheme defaultLabel:@"" withFrame:ShrinkRectWithBorder(rect, sb)];
    label.backgroundColor = cellColorOdd;

    return label;
}

- (UIColor *)backgroundColorForIndex:(NSUInteger)index selected:(BOOL)selected
{
    UIColor  *last      = selected ? [UIColor colorWithRed:0.0 green:0.0 blue:0.0 alpha:0.5] :
                                     [UIColor colorWithRed:0.0 green:0.0 blue:0.0 alpha:0.0];
    UIColor  *other     = selected ? cellHighlightColor :
                                        ((index % 2) ? cellColorOdd :cellColorEven);
    return (!self.delegate || (index == [self.delegate listCount])) ? last : other;
}

- (void)easyTableView:(EasyTableView *)easyTableView setDataForView:(UIView *)view forIndex:(NSUInteger)index
{
    NSString *text = @"";
    if (self.delegate)
    {
        BOOL last = (index == [self.delegate listCount]) && (self.onAdd != nil);
        text = last ? @"add" : [self.delegate listNameAtIndex:index];
        ((UILabel *)view).font = [UITheme defaultFontWithSize:/*last ? 24.0 : [UITheme defaultLabelFontSize]*/24.0];
    }
    ((UILabel *)view).text = text;
    ((UILabel *)view).backgroundColor = [self backgroundColorForIndex:index selected:NO];
}

- (void)easyTableView:(EasyTableView *)easyTableView
         selectedView:(UIView *)selView atIndex:(NSUInteger)selIndex
       deselectedView:(UIView *)desView deselectedIndex:(NSUInteger)desIndex
{
    ((UILabel *)selView).backgroundColor = [self backgroundColorForIndex:selIndex selected:YES];
    ((UILabel *)desView).backgroundColor = [self backgroundColorForIndex:desIndex selected:NO];
}

- (NSUInteger)numberOfCellsForEasyTableView:(EasyTableView *)view
{
    if (self.delegate)
    {
        NSUInteger count = ((NSUInteger)[self.delegate listCount]);
        return count + (self.onAdd ? 1 : 0);
    }
    return 0;
}

- (void)userSelectedViewAtIndexPath:(NSIndexPath *)indexPath
{
    if (self.delegate)
    {
        int index = indexPath.row;
        if (index == [self.delegate listCount])
        {
            self.selectedObject = nil;
            self.selected = -1;
            // then it's the add button
            [SimpleCall call:self.onAdd];
        }
        else
        {
            self.selectedObject = [self.delegate listObjectAtIndex:index];
            [SimpleCall call:self.onSelected withObject:self.selectedObject];
            self.selected = index;
            [self.delegate listSelectIndex:index];
        }
    }
}

- (BOOL)canUserSelectIndexPath:(NSIndexPath *)indexPath
{
    return YES;
}

- (void)update
{
    if (self.delegate)
    {
        [self.delegate listUpdate];
        PerformOnMainThread(table.tableView, @selector(reloadData), nil, NO);
    }
}

- (void)onHiddenChange
{
    [self update];
}

@end