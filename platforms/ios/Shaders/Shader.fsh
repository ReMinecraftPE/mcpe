//
//  Shader.fsh
//  minecraftpe
//
//  Created by Brent on 10/12/23.
//  Copyright (c) 2023 __MyCompanyName__. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
