//
//  AppPlatform_iOS.m
//  Minecraft
//
//  Created by Brent on 10/30/23.
//  Copyright (c) 2023 __MyCompanyName__. All rights reserved.
//

#include "AppPlatform_iOS.h"

#include <fstream>

#include "common/Utils.hpp"
#include "common/Util.hpp"

#include "platforms/audio/openal/CustomSoundSystem.hpp"

AppPlatform_iOS::AppPlatform_iOS(minecraftpeViewController *viewController)
{
	m_bShiftPressed[0] = false;
	m_bShiftPressed[1] = false;
	
	m_bIsKeyboardShown = false;
	
	m_pLogger = new Logger;
	m_pSoundSystem = nullptr;
	
	m_pViewController = viewController;
}

void AppPlatform_iOS::initSoundSystem()
{
	if (!m_pSoundSystem)
	{
		LOG_I("Initializing OpenAL SoundSystem...");
		m_pSoundSystem = new SoundSystemOAL();
	}
	else
	{
		LOG_E("Trying to initialize SoundSystem more than once!");
	}
}

AppPlatform_iOS::~AppPlatform_iOS()
{
	SAFE_DELETE(m_pSoundSystem);
	
	// DELETE THIS LAST
	SAFE_DELETE(m_pLogger);
}

int AppPlatform_iOS::checkLicense()
{
	// we own the game!!
	return 1;
}

int AppPlatform_iOS::getScreenWidth() const
{
	return m_pViewController.width;
}

int AppPlatform_iOS::getScreenHeight() const
{
	return m_pViewController.height;
}

void AppPlatform_iOS::loadImage(ImageData& data, const std::string& path)
{
	std::string realPath = getAssetPath(path);
	
	UIImage * image = [UIImage imageWithContentsOfFile:
						[NSString stringWithUTF8String: realPath.c_str()]];
	
	if (!image || !image.CGImage)
	{
		LOG_E("Couldn't find file: %s", path.c_str());
		return;
	}
	
	data.m_width = CGImageGetWidth(image.CGImage);
	data.m_height = CGImageGetHeight(image.CGImage);
	data.m_data = (uint8_t*)new uint32_t[data.m_width * data.m_height];
	
	CGColorSpace *colorSpace = CGColorSpaceCreateDeviceRGB();
	CGContextRef context = CGBitmapContextCreate(data.m_data, data.m_width, data.m_height, 8u, sizeof(uint32_t) * data.m_width, colorSpace, 0x4001u);
	CGColorSpaceRelease(colorSpace);
	
	CGRect rect;
	rect.origin.x = 0.0;
	rect.origin.y = 0.0;
	rect.size.width = static_cast<float>(data.m_width);
	rect.size.height = static_cast<float>(data.m_height);
	CGContextClearRect(context, rect);
	CGContextTranslateCTM(context, 0.0, 0.0);
	CGContextDrawImage(context, rect, image.CGImage);
	CGContextRelease(context);
}

bool AppPlatform_iOS::shiftPressed()
{
	return m_bShiftPressed[0] || m_bShiftPressed[1];
}

void AppPlatform_iOS::setShiftPressed(bool b, bool isLeft)
{
	m_bShiftPressed[isLeft ? 0 : 1] = b;
}

void AppPlatform_iOS::showKeyboard()
{
	[m_pViewController showKeyboard];
	m_bIsKeyboardShown = true;
}

void AppPlatform_iOS::hideKeyboard()
{
	[m_pViewController hideKeyboard];
	m_bIsKeyboardShown = false;
}

int AppPlatform_iOS::getKeyboardUpOffset()
{
	// @TODO
	// For now we'll just return 1/2 of the screen height. That ought to cover most cases.
	return m_pViewController.height / 2;
}

int AppPlatform_iOS::getUserInputStatus()
{
	return -1;
}

bool AppPlatform_iOS::isTouchscreen() const
{
	return true;
}

bool AppPlatform_iOS::hasFileSystemAccess()
{
	return true;
}

NSString* AppPlatform_iOS::_getBundleResourcePath(const std::string &path) const
{
	size_t dotPos = path.rfind(".", -1, 1);
	size_t slashPos = path.rfind("/", -1, 1);
	size_t dotPos2 = path.rfind('.', -1);
	std::string fileName;
    std::string fileDir = path.substr(0, slashPos + 1);
	std::string fileExtension = dotPos2 != std::string::npos ? path.substr(dotPos2+1, path.length()-dotPos2) : "";
	if ((slashPos & dotPos) != std::string::npos)
	{
		// "." & "/" exist in path
		fileName = path.substr(slashPos+1, dotPos - (slashPos+1));
	}
	else
	{
		fileName = path;
	}
    
    return [[NSBundle mainBundle]
                pathForResource:[NSString stringWithUTF8String:fileName.c_str()]
                ofType:[NSString stringWithUTF8String:fileExtension.c_str()]
                inDirectory: [NSString stringWithUTF8String:fileDir.c_str()]];
}

std::string AppPlatform_iOS::getAssetPath(const std::string &path) const
{
    NSString* assetPath = _getBundleResourcePath(AppPlatform::getAssetPath(path));
    if (assetPath == nullptr)
    {
        // asset couldn't be found in bundle's resources.
        // fallback to the desired path so our error message is clear.
        return path;
    }
    
    return [assetPath UTF8String];
}

void AppPlatform_iOS::showMessageModal(const MessageModal& msg)
{
	NSString *title;
	switch(msg.type)
	{
		case MessageModal::TYPE_ERROR:
			title = @"Error";
			break;
		default:
			LOG_W("Unhandled MessageModal type");
			// fall through
		case MessageModal::TYPE_INFO:
			title = @"Info";
			break;
	}

	UIAlertView *alert = [[UIAlertView alloc]
		initWithTitle:title
		message:[NSString stringWithCString:msg.text.c_str()
		encoding:[NSString defaultCStringEncoding]]
		delegate:nil
		cancelButtonTitle:@"OK"
		otherButtonTitles:nil
	];
	[alert show];
	[alert release];
}
