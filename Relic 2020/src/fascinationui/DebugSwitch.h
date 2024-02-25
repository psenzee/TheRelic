//
//  DebugSwitch.h
//
//  Created by Paul Senzee on 8/30/14.
//  Copyright (c) 2014 ___FULLUSERNAME___. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "GlobalSetters.h"

@class SimpleCall;

// boolean
@interface DebugSwitch : UIView
{
    UILabel          *label;
    UISwitch         *toggle;
    ValueMutator      mutator;
    NSString         *units;
    NSString         *name;
}

@property (nonatomic, retain) SimpleCall *onChange;

- (id)initWithFrame:(CGRect)frame mutator:(ValueMutator)mutator;

@end
