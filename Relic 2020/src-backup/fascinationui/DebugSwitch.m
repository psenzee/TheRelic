//
//  DebugSwitch.mm
//

#import "DebugSwitch.h"

#import <UIKit/UIKit.h>
#import "SimpleCall.h"
#import "SysUtil.h"
#import "RectUtil.h"
#import "UITheme.h"
#import <stdlib.h>
#import "GlobalSetters.h"
#import "THLabel.h"

#define USING_THLABEL 0

@implementation DebugSwitch

- (void)updateText
{
    label.text = [NSString stringWithFormat:@"%@ [%@]", name, (mutator.get_discrete)() ? @"Y" : @"N"];
}

- (void)update
{
    toggle.on = (mutator.get_discrete)() > 0;
    [self updateText];
}

- (void)changed
{
    (mutator.set_discrete)((int)toggle.on);
    [self updateText];
    [SimpleCall call:self.onChange withObject:toggle];
}

- (id)initWithFrame:(CGRect)frame mutator:(ValueMutator)mutator_
{
    self = [super initWithFrame:frame];
    if (self)
    {
        mutator = mutator_;
        
        frame = GrowRect(ZeroOrigin(frame), -5.0);
        CGFloat controlwidth = 60.0;
        CGRect labelframe  = RectInsideAt(frame, RectLeft, frame.size.width - controlwidth);
        CGRect controlframe = RectInsideAt(frame, RectRight, controlwidth);
        
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
        
        toggle = [[UISwitch alloc] initWithFrame:controlframe];
        [self addSubview:toggle];
        
        [toggle addTarget:self action:@selector(changed) forControlEvents:UIControlEventValueChanged];
        [self addSubview:label];
        
        toggle.on = (mutator.get_discrete)() != 0;
        [self updateText];
        
        if ((iPadIdiom() && [name rangeOfString:@"iPhone"].location != NSNotFound) ||
            (!iPadIdiom() && [name rangeOfString:@"iPad"].location != NSNotFound))
        {
            toggle.enabled = NO;
            label.textColor = [UIColor grayColor];
        }
        
        [UITheme registerUIUpdate:[SimpleCall callWithSelector:@selector(update) forTarget:self]];
    }
    return self;
}

@end
