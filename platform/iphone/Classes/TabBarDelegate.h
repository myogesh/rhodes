//
//  TabBarDelegate.h
//  rhorunner
//
//  Created by lars on 8/14/09.
//  Copyright 2009 Home. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "RhoDelegate.h"
#import "NativeBar.h"
#import "BarItem.h"

@interface TabBarDelegate : RhoDelegate <UITabBarControllerDelegate> 
{
@private
	UITabBarController* tabBarController;
	NativeBar* tabBar;
	UIWindow* mainWindow;
	NSMutableArray* barItems;
}

@property(nonatomic, assign) NativeBar* tabBar;
@property(nonatomic, retain) UITabBarController* tabBarController;
@property(nonatomic, retain) UIWindow* mainWindow;
@property(nonatomic, retain) NSMutableArray* barItems;

- (void)createTabBar:(UIWindow*)window;
- (void)loadTabBarItemFirstPage:(BarItem*)item;
- (void)tabBarController:(UITabBarController *)tabBarController didSelectViewController:(UIViewController *)viewController;
@end
