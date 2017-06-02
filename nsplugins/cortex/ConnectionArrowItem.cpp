/*
 * ConnectionArrowItem.cpp
 *
 *  Created on: 30/5/2017
 *      Author: juanpebm
 */
#define _USE_MATH_DEFINES
#include <cmath>

#include "ConnectionArrowItem.h"

namespace nslib
{
  namespace cortex
  {

     const auto M_PI_3 = float( M_PI ) * 0.33f;

    ConnectionArrowItem::ConnectionArrowItem( const ConnectionArrowRep& connectionArrowRep )
    {
      this->_parentRep =
        &( const_cast< ConnectionArrowRep& >( connectionArrowRep ));
    }

    void ConnectionArrowItem::createArrow( const QPointF& origin,
                                           const QPointF& dest )
    {

      QPolygonF arrowShape;
			float arrowWidth = 30;
			float arrowLength = 30;

			QLineF auxLine( origin, dest );

			double angle = ::acos(auxLine.dx( ) / auxLine. length( ));
			if ( auxLine.dy( ) >= 0 )
        angle = ( M_PI * 2.0 ) - angle;

			QPointF arrowInit = auxLine.pointAt(
        1.0f - (arrowLength / auxLine.length( )));
      QPointF arrowP1 = arrowInit -
        QPointF( sin( angle + M_PI_3 ) * arrowWidth,
                 cos( angle + M_PI_3 ) * arrowWidth );
			QPointF arrowP2 = arrowInit -
        QPointF(sin(angle + M_PI - M_PI_3) * arrowWidth,
                cos( angle + M_PI - M_PI_3 ) * arrowWidth);

			arrowShape.clear( );
			arrowShape << auxLine.p1( )
                 << arrowInit
                 << arrowP1
                 << auxLine.p2( )
                 << arrowP2
                 << arrowInit;

			this->setPolygon( arrowShape );
    }
  }
}
