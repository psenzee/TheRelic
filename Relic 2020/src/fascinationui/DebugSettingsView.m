//
//  DebugSettingsView.mm
//

#import "DebugSettingsView.h"

#import <UIKit/UIKit.h>
#import "SimpleCall.h"
#import "SysUtil.h"
#import "RectUtil.h"
#import "UITheme.h"
#import "GlobalSetters.h"
#import "DebugSlider.h"
#import "DebugSwitch.h"
#import "Transitioner.h"

#import <stdlib.h>

@implementation DebugSettingsView

- (void)buttonPressedWithID:(NSString *)str
{
    [SimpleCall call:self.onReturnPressed withObject:str];
}

- (void)addKey:(NSString *)key toButton:(UIButton *)button
{
    SimpleCall *c = [Closure close:@selector(buttonPressedWithID:) forTarget:self withObject:key];
    [button addTarget:c action:@selector(call) forControlEvents:UIControlEventTouchUpInside];
}

- (NSString *)getPreviousCategory:(NSString *)cat
{
    NSString *prev = nil;
    for (NSString *s in categories)
    {
        if ([cat isEqualToString:s])
            return prev;
        prev = s;
    }
    return nil; // not found
}

- (NSString *)getNextCategory:(NSString *)cat
{
    NSString *prev = nil;
    for (NSString *s in categories)
    {
        if ([cat isEqualToString:prev])
            return s;
        prev = s;
    }
    return nil; // not found
}

- (void)showViewForCategory:(NSString *)category
{
    if (category)
    {
        UIView *v = [views objectForKey:category];
        currentCategory = category;
        [transitioner showScreen:v];
    }
}

- (void)goNext
{
    NSString *next = [self getNextCategory:currentCategory];
    if (next) [self showViewForCategory:next];
}

- (void)goPrev
{
    NSString *prev = [self getPreviousCategory:currentCategory];
    if (prev) [self showViewForCategory:prev];
}

- (void)showFirstView
{
    if (categories.count > 0)
    {
        NSString *category = [categories objectAtIndex:0];
        [self showViewForCategory:category];
    }
}

- (void)addDebugControlViewForCategory:(NSString *)category
{
    UIView *view = [[UIView alloc] initWithFrame:self.frame];
    
    CGFloat height = 30.0;
    CGRect frame = RectInsideAt(self.frame, RectTop, height);
    frame.origin.y += height * 1.5; // allow room for screen title
    UILabel *title = [UITheme defaultLabel:category withFrame:frame];
    frame.origin.y += height * 1.0; // allow room for page title
    title.textAlignment = UITextAlignmentCenter;
    [view addSubview:title];
    
    const ValueMutator *mutators = GetGlobalMutators();
    for (int i = 0; mutators[i].name; i++)
    {
        const ValueMutator *m = &mutators[i];
        NSString *cat = !m->category ? @"" : [[NSString alloc] initWithUTF8String:m->category];
        if (m->settable && [cat isEqualToString:category])
        {
            UIView *controller = nil;
            switch (mutators[i].type)
            {
            case ValueType_Real: controller = [[DebugSlider alloc] initWithFrame:frame mutator:mutators[i]]; break;
            case ValueType_Bool: controller = [[DebugSwitch alloc] initWithFrame:frame mutator:mutators[i]]; break;
            default: controller = nil; break;
            }
            if (controller)
            {
                [view addSubview:controller];
                frame.origin.y += height;
            }
        }
    }
    frame.origin.y += height * 0.5;
    frame.size.height += height * 0.5; // add more height
    NSString *prev = [self getPreviousCategory:category];
    NSString *next = [self getNextCategory:category];
    if (prev)
    {
        NSString *text = [NSString stringWithFormat:@"< %@", prev];
        UIButton *b = [UITheme defaultButton:text withFrame:GrowRectWithBorder(RectHalf(frame, RectLeft), MakeEqualBorder(-5.0))];
        [b addTarget:self action:@selector(goPrev) forControlEvents:UIControlEventTouchUpInside];
        [view addSubview:b];
    }
    if (next)
    {
        NSString *text = [NSString stringWithFormat:@"%@ >", next];
        UIButton *b = [UITheme defaultButton:text withFrame:GrowRectWithBorder(RectHalf(frame, RectRight), MakeEqualBorder(-5.0))];
        [b addTarget:self action:@selector(goNext) forControlEvents:UIControlEventTouchUpInside];
        [view addSubview:b];
    }
    [views setObject:view forKey:category];
    [self addSubview:view];
    [transitioner registerScreen:view];
    view.hidden = YES;
}

- (void)addDebugControlViews
{
    for (NSString *category in categories)
        [self addDebugControlViewForCategory:category];
}

- (void)addCategories
{
    const char **gc = GetGlobalCategories();
    for (int i = 0; gc[i]; i++)
        [categories addObject:[NSString stringWithFormat:@"%s", gc[i]]];
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self)
    {
        categories = [[NSMutableArray alloc] init];
        views = [[NSMutableDictionary alloc] init];
        transitioner = [[Transitioner alloc] init];
        
        [self addCategories];
        
        UILabel *title = [UITheme defaultTitleLabel:@"Debug Settings" withFrame:RectInsideAt(frame, RectTop, 40.0)];
        title.font = [UITheme defaultFontWithSize:24.0];
        title.backgroundColor = [UIColor colorWithRed:0.0 green:0.0 blue:0.0 alpha:0.5];
        [self addSubview:title];
        
        [self addDebugControlViews];
        
        UIButton *button = [UIButton buttonWithType:UIButtonTypeCustom];
        button.backgroundColor = [UIColor colorWithRed:0.0 green:0.0 blue:0.0 alpha:0.5];
        [button setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        [button setTitle:@"Return" forState:UIControlStateNormal];
        button.titleLabel.font = [UITheme defaultFontWithSize:24.0];
        button.frame = RectInsideAt(frame, RectBottom, 40.0);
        [self addKey:@"RESUME_PLAY" toButton:button];
        [self addSubview:button];
        
        [self showFirstView];
    }
    return self;
}



@end
