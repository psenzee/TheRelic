//
//  SoundsListView.mm
//
//  Created by Paul Senzee on 8/30/14.
//  Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#import "SoundsListView.h"

#import <UIKit/UIKit.h>
#import "RectUtil.h"
#import "SimpleCall.h"
#import "UITheme.h"
#import "BackgroundView.h"
#import "EasyTableView.h"

@implementation SoundsListView

- (void)hide
{
    self.hidden = YES;
}

- (BOOL)buttonIsDefault:(int)buttonid
{
    if (self.delegate)
        return [self.delegate isDefaultSound:buttonid];
    return YES;
}

- (UIColor *)buttonColor:(BOOL)disabled
{
    return [UIColor colorWithRed:0.0 green:0.75 blue:0.0 alpha:(disabled ? 0.5 : 1.0)];
}

- (void)update
{
    /*
    int index = 0;
    for (UIButton *b in buttons)
    {
        if (index < 8)
            b.backgroundColor = [self buttonColor:[self buttonIsDefault:index]];
        index++;
    }
     */
}

- (void)pressed:(int)index
{
    NSLog(@"PRESSED %d\n", index);
    if (self.delegate)
        return [self.delegate setSelectedSound:index];
}

- (void)pressed1 { [self pressed:1]; }
- (void)pressed2 { [self pressed:2]; }
- (void)pressed3 { [self pressed:3]; }
- (void)pressed4 { [self pressed:4]; }
- (void)pressed5 { [self pressed:5]; }
- (void)pressed6 { [self pressed:6]; }
- (void)pressed7 { [self pressed:7]; }
- (void)pressed8 { [self pressed:8]; }
- (void)pressed9 { [self pressed:9]; }

- (SEL *)buttonSelectors
{
    static SEL SELS[9] = { 0 };
    SEL sels[] =
    {
        @selector(pressed1), @selector(pressed2), @selector(pressed3),
        @selector(pressed4), @selector(pressed5), @selector(pressed6),
        @selector(pressed7), @selector(pressed8), @selector(pressed9),
    };
    if (!SELS[0])
        memcpy(SELS, sels, sizeof(sels));
    return SELS;
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self)
    {
        [self addSubview:[[BackgroundView alloc] initWithFrame:frame]];
        
        UILabel *title = [UITheme defaultTitleLabel:@"Calibrate Sound" withFrame:RectInsideTop(self.frame, 60.0)];
        [self addSubview:title];
        title.backgroundColor = [UITheme defaultDarkBackgroundColor];
        
        Border sborder = MakeBorder(20.0, 20.0, 80.0, 80.0);
        CGRect fr = ShrinkRectWithBorder(frame, sborder);
        EasyTableView *table = [[EasyTableView alloc] initWithFrame:fr numberOfRows:10 ofHeight:60.0];
        table.backgroundColor = [UIColor clearColor];
        table.tableView.backgroundColor = [UIColor clearColor];
   //     table.tableView.backgroundColor = [UITheme defaultDarkBackgroundColor];
        table.delegate = self;
        [self addSubview:table];
        /*
        Border sborder = MakeBorder(20.0, 20.0, 100.0, 100.0);
        buttons = [[NSMutableArray alloc] initWithCapacity:9];
        CGRect fr = ShrinkRectWithBorder(frame, sborder);
        Border bborder = MakeBorder(2.0, 2.0, 2.0, 2.0);
        for (int i = 0; i < 9; i++)
        {
            CGRect r = RectForGridPosition(fr, 3, 3, i % 3, i / 3);
            UIButton *b = [UITheme defaultButton:@"" withFrame:ShrinkRectWithBorder(r, bborder)];
            NSString *title = @"+";
            if (i != 8)
                title = [NSString stringWithFormat:@"%d", i + 1];
            else
                b.backgroundColor = [UIColor redColor];
            [b setTitle:title forState:UIControlStateNormal];
            [self addSubview:b];
            [b addTarget:self action:([self buttonSelectors])[i] forControlEvents:UIControlEventTouchUpInside];
            [buttons addObject:b];
        }
        [self update];
        */
        
        [self addSubview:(done = [UITheme defaultButton:@"Cancel" withFrame:RectInsideBottom(frame, 60.0)])];
        
        [done addTarget:self action:@selector(hide) forControlEvents:UIControlEventTouchUpInside];
    }
    return self;
}

- (UIView *)easyTableView:(EasyTableView *)easyTableView viewForRect:(CGRect)rect
{
    Border sb = MakeBorder(1.0, 1.0, 1.0, 1.0);
    UILabel *label = [UITheme defaultLabel:@"" withFrame:ShrinkRectWithBorder(rect, sb)];
    label.backgroundColor = [UIColor colorWithRed:0.5 green:0.0 blue:0.0 alpha:1.0];
    return label;
}

- (void)easyTableView:(EasyTableView *)easyTableView setDataForView:(UIView *)view forIndex:(NSUInteger)index
{
    NSString *str = [NSString stringWithFormat:@"sample #%d", index + 1];
    ((UILabel *)view).text = str;
}

@end
