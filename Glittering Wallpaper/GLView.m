//
//  GLView.m
//  FlappyBird
//
//  Created by mutexre on 21/11/15.
//  Copyright © 2015 mutexre. All rights reserved.
//

#import "GLView.h"

@interface GLView ()
@end

@implementation GLView

+ (Class)layerClass {
    return [CAEAGLLayer class];
}

- (BOOL)setupContext {
    context = [GLView createBestContext];
    
    if (!context || ![EAGLContext setCurrentContext:context])
        return NO;
    
    if (context.API == kEAGLRenderingAPIOpenGLES3)
        isES3 = YES;
    
    return YES;
}

- (void)setupLayer:(BOOL)opaque retained:(BOOL)retained colorFormat:(NSString*)colorFormat {
    CAEAGLLayer* layer = (CAEAGLLayer*)self.layer;
    layer.opaque = opaque;
    layer.drawableProperties = @{
        kEAGLDrawablePropertyRetainedBacking : [NSNumber numberWithBool:retained],
        kEAGLDrawablePropertyColorFormat : colorFormat
    };
}

- (BOOL)setupFramebuffer
{
    [self setCurrentContext];
    [self deleteFramebuffer];

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glGenRenderbuffers(1, &renderbuffer.color);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer.color);

    if (![context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*)self.layer]) {
        NSLog(@"Failed to bind CAEAGLLayer storage to renderbuffer");
        return NO;
    }

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderbuffer.color);

    GLint w, h;
    [GLView getRenderbufferWidth:&w height:&h];
    printf("renderbuffer %u x %u\n", w, h);

    glGenRenderbuffers(1, &renderbuffer.depth);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer.depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer.depth);

    GLenum fbStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fbStatus != GL_FRAMEBUFFER_COMPLETE) {
        NSLog(@"Framebuffer is imcomplete, status %u", fbStatus);
        glDeleteRenderbuffers(1, &renderbuffer.color);
        glDeleteRenderbuffers(1, &renderbuffer.depth);
        glDeleteFramebuffers(1, &framebuffer);
        return NO;
    }

    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer.color);

    return YES;
}

- (void)deleteFramebuffer {
    if (framebuffer) {
        glDeleteFramebuffers(1, &framebuffer);
        framebuffer = 0;
    }

    if (renderbuffer.color) {
        glDeleteRenderbuffers(1, &renderbuffer.color);
        renderbuffer.color = 0;
    }

    if (renderbuffer.depth) {
        glDeleteRenderbuffers(1, &renderbuffer.depth);
        renderbuffer.depth = 0;
    }
}

- (BOOL)setup:(BOOL)opaque
     retained:(BOOL)retained
  colorFormat:(NSString*)colorFormat
{
    if (![self setupContext])
        return NO;
    
    [self setupLayer:opaque
            retained:retained
         colorFormat:colorFormat];
    
    return YES;
}

- (instancetype)initWithFrame:(CGRect)frame
                       opaque:(BOOL)opaque
                     retained:(BOOL)retained
                  colorFormat:(NSString*)colorFormat
{
    if (self = [super initWithFrame:frame]) {
        if (![self setup:opaque retained:retained colorFormat:colorFormat])
            return nil;
    }
    return self;
}

- (instancetype)init
{
    return [self initWithFrame:CGRectZero
                        opaque:YES
                      retained:NO
                   colorFormat:kEAGLColorFormatRGBA8];
}

- (instancetype)initWithCoder:(NSCoder*)decoder
{
    if (self = [super initWithCoder:decoder]) {
        if (![self setup:YES retained:NO colorFormat:kEAGLColorFormatRGBA8])
            return nil;
    }
    return self;
}

- (void)dealloc {
    [self deleteFramebuffer];
}

- (void)setupScale {
    NSString* systemVersion = [[UIDevice currentDevice] systemVersion];
    NSComparisonResult order = [systemVersion compare:@"8.0" options:NSNumericSearch];

    UIScreen* screen = self.window.screen;
    float scale;
    if (order == NSOrderedAscending)
        scale = screen.scale;
    else
        scale = screen.nativeScale;

    if (self.contentScaleFactor != scale)
        self.contentScaleFactor = scale;
}

- (void)layoutSubviews {
    [super layoutSubviews];

    [self setupScale];
    [self setupFramebuffer];

    CGRect f = self.frame;
    float scale = self.contentScaleFactor;
    f.origin.x *= scale;
    f.origin.y *= scale;
    f.size.width *= scale;
    f.size.height *= scale;
    [_delegate frameSizeChanged:f];
}

- (BOOL)isES3 {
    return isES3;
}

- (void)displayLinkCallback:(CADisplayLink*)dlink {
    [self setCurrentContext];
    [_delegate drawGlView:self t:dlink.timestamp];
    if (![context presentRenderbuffer:GL_RENDERBUFFER])
        NSLog(@"Failed to present renderbuffer");

#if DEBUG
    [GLView checkGlErrors];
#endif
}

- (BOOL)animated {
    return displayLink != nil;
}

- (void)setAnimated:(BOOL)animated {
    if (animated)
        [self startAnimation];
    else
        [self stopAnimation];
}

- (void)startAnimation {
    if (!displayLink) {
        displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(displayLinkCallback:)];
        displayLink.frameInterval = 1;
        [displayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];
    }
}

- (void)stopAnimation {
    if (displayLink) {
        [displayLink invalidate];
        displayLink = nil;
    }
}

- (void)setCurrentContext {
    [EAGLContext setCurrentContext:context];
}

#pragma mark Helper methods

+ (EAGLContext*)createBestContext {
    EAGLContext* context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    if (!context) context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    return context;
}

+ (void)checkGlErrors {
    BOOL errorFound = NO;
    GLenum glError;
    do {
        glError = glGetError();
        if (glError != GL_NO_ERROR) {
            printf("%u ", glError);
            errorFound = YES;
        }
    }
    while (glError != GL_NO_ERROR);
    if (errorFound) printf("\n");
}

+ (void)getRenderbufferWidth:(GLint*)w height:(GLint*)h {
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, w);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, h);
}

@end
