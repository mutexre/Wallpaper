//
//  ViewController.m
//  Wallpaper
//
//  Created by mutexre on 04/09/16.
//
//

#import <string>
#import <array>
#import "ViewController.h"
#import "Scene.hpp"
#import "Option.hpp"
#import <glm/gtx/color_space.hpp>

using namespace std;
using namespace glm;

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
    
    int rows = ceil(h / cellHeight) + 4;
    int columns = ceil(w / cellWidth) + 8;
    
    shared_ptr<Grid> grid = make_shared<Grid>(rows, columns, 3, cellWidth);
    grid->init();
    
    vec2 bottomLeft = grid->getVertexCoord(0, 0);
    vec2 topRight = grid->getVertexCoord(rows, columns);
    vec2 center = bottomLeft + 0.5f * (topRight - bottomLeft);
    
    array<vec4, 3> pointColors = {
        vec4(rgbColor(vec3(0, 0, 1.00)), 1),
        vec4(rgbColor(vec3(0, 0, 0.75)), 1),
        vec4(rgbColor(vec3(0, 0, 0.5)), 1)
    };
    
    array<vec4, 2> lineColors = {
        vec4(rgbColor(vec3(0, 0, 0.7 )), 1),
        vec4(rgbColor(vec3(0, 0, 0.15)), 1)
    };
    
    vec4 trianglesColor = vec4(rgbColor(vec3(0, 0, 0.25)), 1);
    
    scene = unique_ptr<Scene>(new Scene(grid, pointColors, lineColors, trianglesColor));
    scene->setScale(vec2(1));
    scene->setTranslate(-center + vec2(0., 0.));
}

- (void)initGlOptions
{
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glDepthRangef(0.f, 1.f);
    glClearDepthf(1.f);
    glLineWidth(2.f);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    float c = 0.;
    glClearColor(c * 124./255., c * 135.f/255.f, c * 157.f/255.f, 1);
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    [self.glView setCurrentContext];
    [self initScene];
    [self initGlOptions];
    
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
    
    if (!startTime.defined)
        startTime.define(t);
    
    scene->render(t - startTime.value);
}

- (void)frameSizeChanged:(CGRect)rect
{
    float aspect = rect.size.width / rect.size.height;
    scene->setAspectRatio(aspect);
    glViewport(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
}

@end
