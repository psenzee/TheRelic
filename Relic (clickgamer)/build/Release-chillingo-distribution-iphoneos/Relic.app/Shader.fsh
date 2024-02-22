//
//  Shader.fsh
//  ChillingoCrystalTest
//
//  Created by Paul Senzee on 1/11/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
