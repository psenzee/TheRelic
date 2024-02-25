//
//  WebView.h
//

#import <UIKit/UIKit.h>

@class SimpleCall;

@interface WebView : UIView <UIWebViewDelegate>
{
    UIButton      *done;
    UIButton      *del;
    UIButton      *test;
    UILabel       *title;
}

@property (nonatomic, retain) SimpleCall       *onAdd;
@property (nonatomic, retain) SimpleCall       *onDone;
@property (nonatomic, retain) SimpleCall       *onDelete;
@property (nonatomic, retain) SimpleCall       *onTest;
@property (nonatomic, retain) SimpleCall       *onSelected;
@property (nonatomic, retain) id                selectedObject;
@property (nonatomic)         int               selected;

@property (nonatomic, retain) UIWebView        *webview;

- (id)initWithFrame:(CGRect)frame withTitle:(NSString *)title;
- (void)setTitle:(NSString *)title;

- (void)loadURLString:(NSString *)url;

@end
