//
//  Shader.fsh
//  minecraftpe
//
//  Created by Brent on 10/12/23.
//  Copyright (c) 2023 ReMinecraftPE. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
