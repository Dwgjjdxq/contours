###############################################################################
#                                                                             #
# This file is part of a style sheet example from the Freestyle application   #
# Copyright (C) 2001-2005  Stephane Grabli                                    #
#                                                                             #
# http://freestyle.sourceforge.net                                            #
#                                                                             #
###############################################################################

from Freestyle import *
from logical_operators import *
from PredicatesB1D import *
from shaders import *

Operators.select(QuantitativeInvisibilityUP1D(0))
Operators.bidirectionalChain(ChainSilhouetteIterator(), NotUP1D(QuantitativeInvisibilityUP1D(0)))
shaders_list = 	[
		StrokeTextureShader("smoothAlpha.bmp", 
				     Stroke.OPAQUE_MEDIUM, 
				     0),

		pySamplingShader(2),
		ConstantColorShader(1,1,1),
#		IncreasingColorShader(0.2,0.2,0.2,1,0.5,0.5,0.5,1),
		pyNonLinearVaryingThicknessShader(0.5,4,0.6), 
		BSplineShader(),
#		pyMaterialColorShader(0)
		]
Operators.create(TrueUP1D(), shaders_list)
