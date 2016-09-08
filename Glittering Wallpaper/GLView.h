//
//  GLView.h
//  FlappyBird
//
//  Created by mutexre on 21/11/15.
//  Copyright © 2015 mutexre. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <OpenGLES/ES3/gl.h>

@protocol GLViewDelegate;

@interface GLView : UIView {
@private
    EAGLContext* context;
    CADisplayLink* displayLink;
    GLuint framebuffer;
    struct { GLuint color, depth; } renderbuffer;
    BOOL isES3;
}

@property (nonatomic) id<GLViewDelegate> delegate;
@property BOOL animated;
@property (readonly) BOOL isES3;

- (instancetype)initWithFrame:(CGRect)frame
                       opaque:(BOOL)opaque
                     retained:(BOOL)retained
                  colorFormat:(NSString*)colorFormat;

- (void)setCurrentContext;
- (void)deleteFramebuffer;
- (BOOL)setupFramebuffer;

@end

@protocol GLViewDelegate
- (void)drawGlView:(GLView*)sender t:(double)t;
- (void)frameSizeChanged:(CGRect)rect;
@end
