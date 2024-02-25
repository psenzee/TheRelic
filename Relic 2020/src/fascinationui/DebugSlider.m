//
//  DebugSlider.mm
//

#import "DebugSlider.h"

#import <UIKit/UIKit.h>
#import "SimpleCall.h"
#import "SysUtil.h"
#import "RectUtil.h"
#import "UITheme.h"
#import <stdlib.h>
#import "GlobalSetters.h"
#import "THLabel.h"

#define USING_THLABEL 0

@implementation DebugSlider

- (void)updateText
{
    label.text = [NSString stringWithFormat:@"%@ [%.02f%@]", name, (mutator.get_real)(), units];
}

- (void)update
{
    slider.value = (mutator.get_real)();
    [self updateText];
}

- (void)changed
{
    (mutator.set_real)((double)slider.value);
    [self updateText];
    [SimpleCall call:self.onSlid withObject:slider];
}

- (id)initWithFrame:(CGRect)frame mutator:(ValueMutator)mutator_
{
    self = [super initWithFrame:frame];
    if (self)
    {
        mutator = mutator_;
        
        frame = GrowRect(ZeroOrigin(frame), -5.0);
        CGRect labelframe  = RectHalf(frame, RectLeft);
        CGRect controlframe = RectHalf(frame, RectRight);
        
        units = !mutator.units ? @"" : [[NSString alloc] initWithUTF8String:mutator.units];
        name  = !mutator.name  ? @"" : [[NSString alloc] initWithUTF8String:mutator.name];
        
#if USING_THLABEL != 0
        THLabel *thlabel = [[THLabel alloc] initWithFrame:labelframe];
        thlabel.strokeSize = 3.0;
        thlabel.strokeColor = [UIColor blackColor];
        thlabel.strokePosition = THLabelStrokePositionOutside;
        label = thlabel;
#else
        label = [[UILabel alloc] initWithFrame:labelframe];
        label.layer.shadowColor = [[UIColor blackColor] CGColor];
        label.layer.shadowOffset = CGSizeMake(0.0f, 1.0f);
        label.layer.shadowOpacity = 1.0f;
        label.layer.shadowRadius = 3.0f;
#endif
        label.textColor = [UIColor whiteColor];
        label.userInteractionEnabled = NO;
        label.font = [UITheme defaultFontWithSize:12.0];
        
        slider = [[UISlider alloc] initWithFrame:controlframe];
        [self addSubview:slider];
        
        [slider addTarget:self action:@selector(changed) forControlEvents:UIControlEventValueChanged];
        slider.maximumValue = mutator.max;
        slider.minimumValue = mutator.min;
        [self addSubview:label];
        
        slider.value = (mutator.get_real)();
        [self updateText];
        
        if ((iPadIdiom() && [name rangeOfString:@"iPhone"].location != NSNotFound) ||
            (!iPadIdiom() && [name rangeOfString:@"iPad"].location != NSNotFound))
        {
            slider.enabled = NO;
            label.textColor = [UIColor grayColor];
        }
        
        [UITheme registerUIUpdate:[SimpleCall callWithSelector:@selector(update) forTarget:self]];
    }
    return self;
}

@end
