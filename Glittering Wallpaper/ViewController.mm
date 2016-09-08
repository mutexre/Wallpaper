//
//  ViewController.m
//  Glittering Wallpaper
//
//  Created by mutexre on 04/09/16.
//
//

#import <string>
#import "ViewController.h"
#import "Scene.hpp"
#import "Option.hpp"

using namespace std;

@interface ViewController ()
@property (readonly) GLView* glView;
@end

@implementation ViewController
{
    unique_ptr<Scene> scene;
    Option<double> startTime;
}

- (GLView*)glView {
    return (GLView*)self.view;
}

- (void)initScene
{
    CGRect screenBounds = [UIScreen mainScreen].bounds;
    float aspectRatio = screenBounds.size.width / screenBounds.size.height;
    
    float cellWidth = 0.05;
    float cellHeight = cellWidth * sin(M_PI / 3);
    
    float w = 2.0;
    float h = 2.0 / aspectRatio;
    
    int rows = ceil(h / cellHeight);
    int columns = ceil((w / cellWidth) + 0.5 * rows);
    
    float c0 = 1;
    float c1 = 0.5;
    float c2 = 0.25;
    float c4 = 0.125;
    float a = 1;
    
    Model model(rows, columns, 3, cellWidth, 0.0025,
                { c0, c0, c0, a },
                { c1, c1, c1, a },
                { c2, c2, c2, a },
                { c4, c4, c4, a });
    
    vec2 bottomLeft = model.getCoord(0, 0);
    vec2 topRight = model.getCoord(rows, columns);
    vec2 center = bottomLeft + 0.5f * (topRight - bottomLeft);
    
    scene = unique_ptr<Scene>(new Scene(model));
    scene->setScale(vec2(1));
    scene->setTranslate(-center);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    //glEnable(GL_SCISSOR_TEST);
    glDepthRangef(0.f, 1.f);
    glClearDepthf(1.f);
    //glClearColor(0.125, 0.25, 0.5, 1);
    glClearColor(0, 0, 0, 1);
    glLineWidth(1.f);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    [self.glView setCurrentContext];
    [self initScene];
    
    self.glView.delegate = self;
    self.glView.animated = YES;
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

#pragma mark GLViewDelegate

- (void)drawGlView:(GLView*)sender t:(double)t
{
    [self.glView setCurrentContext];
    
    if (!startTime.defined) {
        startTime.define(t);
        t = 0;
    }
    else
        t -= startTime.value;
    
    scene->render(t);
}

- (void)frameSizeChanged:(CGRect)rect
{
    float aspect = rect.size.width / rect.size.height;
    scene->setAspectRatio(aspect);
    glViewport(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
//    glScissor(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
}

@end
