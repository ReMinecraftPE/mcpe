//
//  AppPlatform_iOS.m
//  Minecraft
//
//  Created by Brent on 10/30/23.
//  Copyright (c) 2023 __MyCompanyName__. All rights reserved.
//

#include "AppPlatform_iOS.h"

#include "common/Utils.hpp"

#include "platforms/openal/SoundSystemAL.hpp"

AppPlatform_iOS::AppPlatform_iOS()
{
    //_storageDir = storageDir;
    
    m_bShiftPressed[0] = false;
    m_bShiftPressed[1] = false;
    
    //ensureDirectoryExists(_storageDir.c_str());
	
	m_pLogger = new Logger;
	m_pSoundSystem = nullptr;
}

void AppPlatform_iOS::initSoundSystem()
{
	if (!m_pSoundSystem)
	{
		m_pSoundSystem = new SoundSystemAL();
		LOG_I("Initializing OpenAL SoundSystem...");
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

Texture AppPlatform_iOS::loadTexture(const std::string& path, bool b)
{
	Texture out;
	out.field_C = 1;
	out.field_D = 0;
    
    std::string realPath = getAssetPath(path);
    
    UIImage * image = [UIImage imageWithContentsOfFile:
                        [NSString stringWithUTF8String: realPath.c_str()]];
    
	if (!image || !image.CGImage)
	{
		LOG_E("Couldn't find file: %s", path.c_str());
		return out;
	}
    
	out.m_width = CGImageGetWidth(image.CGImage);
	out.m_height = CGImageGetHeight(image.CGImage);
    
    CGColorSpace *colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef context = CGBitmapContextCreate(out.m_pixels, out.m_width, out.m_height, 8u, 4 * out.m_width, colorSpace, 0x4001u);
    CGColorSpaceRelease(colorSpace);
	
	CGRect rect;
    rect.origin.x = 0.0;
    rect.origin.y = 0.0;
    rect.size.width = static_cast<float>(out.m_width);
    rect.size.height = static_cast<float>(out.m_height);
    CGContextClearRect(context, rect);
    CGContextTranslateCTM(context, 0.0, 0.0);
    CGContextDrawImage(context, rect, image.CGImage);
    CGContextRelease(context);
	
	// Code for layered textures(?) which we currently do not support
    /*if ( (out.var8 & 1) != 0 )
        v19 = *(unsigned __int8 **)(out + 16);
    else
        v19 = (unsigned __int8 *)(out + 9);
    
    int imageSize = out.width * out.height;
    if ( 4 * imageSize >= 1 )
    {
        v21 = &v19[4 * imageSize];
        do
        {
            // Loop until end
            v22 = 255.0 / (float)v19[3];
            *v19 = (int)(float)(v22 * (float)*v19);
            v19[1] = (int)(float)(v22 * (float)v19[1]);
            v19[2] = (int)(float)(v22 * (float)v19[2]);
            v19 += 4;
        }
        while ( v19 < v21 );
    }*/
    
	return out;
}


void AppPlatform_iOS::setMouseGrabbed(bool b)
{
	
}

void AppPlatform_iOS::setMouseDiff(int x, int y)
{
	xrel = x;
	yrel = y;
}

void AppPlatform_iOS::getMouseDiff(int& x, int& y)
{
	x = xrel;
	y = yrel;
}

void AppPlatform_iOS::clearDiff()
{
	xrel = 0;
	yrel = 0;
}

bool AppPlatform_iOS::shiftPressed()
{
	return m_bShiftPressed[0] || m_bShiftPressed[1];
}

void AppPlatform_iOS::setShiftPressed(bool b, bool isLeft)
{
	m_bShiftPressed[isLeft ? 0 : 1] = b;
}

int AppPlatform_iOS::getUserInputStatus()
{
	return -1;
}

std::string AppPlatform::getAssetPath(const std::string &path) const
{
    /*std::string realPath;
    if (realPath.size() && realPath[0] == '/')
    {
        // trim it off
        realPath = realPath.substr(1);
    }*/
    
    size_t dotPos = path.rfind(".", -1, 1);
    size_t slashPos = path.rfind("/", -1, 1);
    size_t dotPos2 = path.rfind('.', -1);
    std::string fileName;
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

    return [[[NSBundle mainBundle] pathForResource:[NSString stringWithUTF8String:fileName.c_str()] ofType:[NSString stringWithUTF8String:fileExtension.c_str()]] UTF8String];
}