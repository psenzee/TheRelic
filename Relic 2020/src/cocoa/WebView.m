//
//  WebView.m
//

#import "WebView.h"

#import <UIKit/UIKit.h>
#import "RectUtil.h"
#import "SimpleCall.h"
#import "UITheme.h"
#import "EasyTableView.h"
#import "UIUtil.h"
#import "SysUtil.h"

extern void ShowSpinner(NSString *);
extern void HideSpinner();

@implementation WebView

- (void)donePressed
{
    [SimpleCall call:self.onDone];
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
        _webview = [[UIWebView alloc] initWithFrame:fr];
        self.webview.backgroundColor = [UIColor clearColor];
//      table.delegate = self;
        [self addSubview:self.webview];
        self.webview.opaque = NO;
        
        self.webview.layer.cornerRadius = RealWidth(20.0);
        self.webview.layer.masksToBounds = YES;
        self.webview.delegate = self;
        
   //     self.webview.scrollView.zoomScale = 2.0;
        
        self.webview.scrollView.scrollEnabled = NO;

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

- (void)loadURLString:(NSString *)url
{
    ShowSpinner(@"Loading..");
    [self.webview loadRequest:
       [NSURLRequest requestWithURL:
          [NSURL URLWithString:url]]];
}

- (void)webViewDidFinishLoad:(UIWebView *)webView
{
    HideSpinner();
}

- (void)setTitle:(NSString *)str
{
    title.text = str;
}

- (void)onHiddenChange
{
    if (!self.hidden)
    {

        
    }
}

@end