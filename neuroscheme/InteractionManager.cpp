/*
 * Copyright (c) 2016 GMRV/URJC/UPM.
 *
 * Authors: Pablo Toharia <pablo.toharia@upm.es>
 *
 * This file is part of NeuroScheme
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */
#include "DataManager.h"
#include "InteractionManager.h"
#include "LayoutManager.h"
#include "PaneManager.h"
#include "RepresentationCreatorManager.h"
#include "SelectionManager.h"
#include "ZeroEQManager.h"
#include "domains/domains.h"

namespace neuroscheme
{

  // QPen InteractionManager::_selectedPen =
  //   QPen( Qt::red, 3, Qt::SolidLine,
  //         Qt::RoundCap, Qt::RoundJoin );

  // QPen InteractionManager::_hoverSelectedPen =
  //   QPen( Qt::red, 3, Qt::DotLine,
  //         Qt::RoundCap, Qt::RoundJoin );

  // QPen InteractionManager::_partiallySelectedPen =
  //   QPen( Qt::yellow, 3, Qt::SolidLine,
  //         Qt::RoundCap, Qt::RoundJoin );

  // QPen InteractionManager::_hoverPartiallySelectedPen =
  //   QPen( Qt::yellow, 3, Qt::DotLine,
  //         Qt::RoundCap, Qt::RoundJoin );

  // QPen InteractionManager::_unselectedPen = QPen( Qt::NoPen );

  // QPen InteractionManager::_hoverUnselectedPen =
  //   QPen( QColor( 200, 200, 200, 255 ), 3, Qt::DotLine,
  //         Qt::RoundCap, Qt::RoundJoin );

  QMenu* InteractionManager::_contextMenu = nullptr;


  void InteractionManager::hoverEnterEvent(
    QAbstractGraphicsShapeItem* item,
    QGraphicsSceneHoverEvent* /* event */ )
  {
    // std::cout << "hover" << std::endl;
    auto selectableItem = dynamic_cast< SelectableItem* >( item );
    if ( selectableItem )
    {
      selectableItem->hover( true );
      selectableItem->setSelected( selectableItem->selectedState( ));
      // if ( selectableItem->selected( ))
      //   item->setPen( _hoverSelectedPen );
      // else if ( selectableItem->partiallySelected( ))
      //   item->setPen( _hoverPartiallySelectedPen );
      // else
      //   item->setPen( _hoverUnselectedPen );
    }
    else
    {
      item->setPen( SelectableItem::hoverUnselectedPen( ));
    }
  }


  void InteractionManager::hoverLeaveEvent(
    QAbstractGraphicsShapeItem* item,
    QGraphicsSceneHoverEvent* /* event */ )
  {
    auto selectableItem = dynamic_cast< SelectableItem* >( item );
    if ( selectableItem )
    {
      selectableItem->hover( false );
      selectableItem->setSelected( selectableItem->selectedState( ));
      // if ( selectableItem->selected( ))
      //   item->setPen( _selectedPen );
      // else if ( selectableItem->partiallySelected( ))
      //   item->setPen( _partiallySelectedPen );
      // else
      //   item->setPen( _unselectedPen );
    }
    else
    {
      item->setPen( SelectableItem::unselectedPen( ));
    }
  }


  void InteractionManager::contextMenuEvent(
    QAbstractGraphicsShapeItem* shapeItem,
    QGraphicsSceneContextMenuEvent* event )
  {

    if ( !_contextMenu )
      _contextMenu = new QMenu( );
    else
      _contextMenu->clear( );

    {
      auto item = dynamic_cast< Item* >( shapeItem );
      if ( item )
      {
        assert( item->parentRep( ));
        const auto& repsToEntities =
          RepresentationCreatorManager::repsToEntities( );
        if ( repsToEntities.find( item->parentRep( )) != repsToEntities.end( ))
        {
          const auto entities = repsToEntities.at( item->parentRep( ));
          auto entityGid = ( *entities.begin( ))->entityGid( );

          auto& relParentOf = *( DataManager::entities( ).
                                 relationships( )[ "isParentOf" ]->asOneToN( ));
          const auto& children = relParentOf[ entityGid ];

          auto& relChildOf = *( DataManager::entities( ).relationships( )
                                [ "isChildOf" ]->asOneToOne( ));
          const auto& parent = relChildOf[ entityGid ];

          const auto& grandParent = relChildOf[ relChildOf[ entityGid ]];

          const auto& parentSiblings = relParentOf[ grandParent ];

          auto& relAGroupOf = *( DataManager::entities( ).relationships( )
                                [ "isAGroupOf" ]->asOneToN( ));
          const auto& groupedEntities = relAGroupOf[ entityGid ];

          QAction* levelUp = nullptr;
          QAction* levelDown = nullptr;
          QAction* expandGroup = nullptr;
          QAction* levelUpToNewPane = nullptr;
          QAction* levelDownToNewPane = nullptr;
          QAction* expandGroupToNewPane = nullptr;

          if ( parent != 0 )
            levelUp = _contextMenu->addAction( QString( "Level up" ));
          if ( children.size( ) > 0 )
            levelDown = _contextMenu->addAction( QString( "Level down" ));
          if ( groupedEntities.size( ) > 0 )
            expandGroup = _contextMenu->addAction( QString( "Expand group" ));

          if ( levelUp || levelDown || expandGroup )
            _contextMenu->addSeparator( );

          if ( parent != 0 )
            levelUpToNewPane =
              _contextMenu->addAction( QString( "Level up [new pane]" ));
          if ( children.size( ) > 0 )
            levelDownToNewPane =
              _contextMenu->addAction( QString( "Level down [new pane]" ));
          if ( groupedEntities.size( ) > 0 )
            expandGroupToNewPane = _contextMenu->addAction(
              QString( "Expand group [new pane]" ));

          if ( levelUp || levelDown || expandGroup ||
               levelUpToNewPane || levelDownToNewPane || expandGroupToNewPane )
          {
            shift::Representations representations;
            shift::Entities targetEntities;
            QAction* selectedAction = _contextMenu->exec( event->screenPos( ));

            if (( levelUpToNewPane &&
                  levelUpToNewPane == selectedAction ) ||
                ( levelDownToNewPane &&
                  levelDownToNewPane == selectedAction ) ||
                ( expandGroupToNewPane &&
                  expandGroupToNewPane == selectedAction ))
            {
              neuroscheme::PaneManager::activePane(
                neuroscheme::PaneManager::newPaneFromActivePane( ));
            }

            if (( levelUp && levelUp == selectedAction ) ||
                ( levelUpToNewPane && levelUpToNewPane == selectedAction ))
            {
              if ( parentSiblings.size( ) > 0 )
                for ( const auto& parentSibling : parentSiblings )
                  targetEntities.add(
                    DataManager::entities( ).at( parentSibling ));
              else
                targetEntities.add( DataManager::entities( ).at( parent ));
            }

            if (( levelDown && levelDown == selectedAction ) ||
                ( levelDownToNewPane && levelDownToNewPane == selectedAction ))
            {
              for ( const auto& child : children )
                targetEntities.add( DataManager::entities( ).at( child ));
            }

            if (( expandGroup && expandGroup == selectedAction ) ||
                ( expandGroupToNewPane &&
                  expandGroupToNewPane == selectedAction ))
            {
              for ( const auto& groupedEntity : groupedEntities )
                targetEntities.add(
                  DataManager::entities( ).at( groupedEntity ));
            }
            std::cout << ",,Target entities " << targetEntities.size( ) << std::endl;
            if ( targetEntities.size( ) > 0 )
            {
              // auto canvas = dynamic_cast< Canvas* >(
              //   shapeItem->scene( )->parent( ));
              // assert( canvas );
              auto canvas = PaneManager::activePane( );
              canvas->displayEntities( targetEntities, false, true );
            }
          }
        }
        else
        {
          Log::log( NS_LOG_HEADER + "item without entity",
                    LOG_LEVEL_ERROR );
          return;
        }
      }
      else
        Log::log( NS_LOG_HEADER + "clicked element is not item",
                  LOG_LEVEL_ERROR );
    }

  }


  void InteractionManager::mousePressEvent(
    QAbstractGraphicsShapeItem* shapeItem,
    QGraphicsSceneMouseEvent* event )
  {
    // Selection event
    if ( event->buttons( ) & Qt::LeftButton )
    {
      auto item = dynamic_cast< Item* >( shapeItem );

      auto selectableItem = dynamic_cast< SelectableItem* >( item );
      if ( selectableItem )
      {
         std::cout << "-------- mousePressEvent " << shapeItem << " "
              <<  item->parentRep( ) << std::endl;

        const auto& repsToEntities =
          RepresentationCreatorManager::repsToEntities( );
        if ( repsToEntities.find( item->parentRep( )) != repsToEntities.end( ))
        {
          const auto entities = repsToEntities.at( item->parentRep( ));
          // auto entityGid = ( *entities.begin( ))->entityGid( );
          if ( selectableItem->partiallySelected( ))
            selectableItem->setSelected( );
          else
            selectableItem->toggleSelected( );

          std::cout << "-------- Selecting entities "
                    << entities.size( ) << std::endl;
          for ( const auto& entity : entities )
          {
            std::cout << "-------- " << entity->entityGid( ) << std::endl;
            // std::cout << "-- ShiFT gid: "
            //           << int( entity->entityGid( )) << std::endl;

            if ( selectableItem->selected( ))
            {
              SelectionManager::setSelectedState(
                entity, SelectedState::SELECTED );
//              shapeItem->setPen( _selectedPen );
            }
            else
            {
//              shapeItem->setPen( _unselectedPen );
              SelectionManager::setSelectedState(
                entity, SelectedState::UNSELECTED );
            }

            auto parentState = SelectionManager::getSelectedState( entity );


            auto entityGid = ( *entities.begin( ))->entityGid( );

            const auto& relSuperEntityOf =
              *( DataManager::entities( ).relationships( )
                 [ "isSuperEntityOf" ]->asOneToN( ));
            if ( relSuperEntityOf.count( entityGid ) > 0 )
            {
              const auto& subEntities = relSuperEntityOf.at( entityGid );
              for ( const auto& subEntity : subEntities )
                SelectionManager::setSelectedState(
                  DataManager::entities( ).at( subEntity ), parentState );
            }

            std::cout << "Propagate to children of " << entityGid << std::endl;
            _propagateSelectedStateToChilds(
              DataManager::entities( ),
              *( DataManager::entities( ).
                 relationships( )[ "isParentOf" ]->asOneToN( )),
              relSuperEntityOf,
              entityGid,
              parentState );

            _propagateSelectedStateToParent(
              DataManager::entities( ),
              *( DataManager::entities( ).
                 relationships( )[ "isChildOf" ]->asOneToOne( )),
              *( DataManager::entities( ).
                 relationships( )[ "isParentOf" ]->asOneToN( )),
              entityGid,
              parentState );

            //std::cout << std::endl;

            //LayoutManager::updateAllScenesSelection( );
            PaneManager::updateSelection( );
          }
        }
        else
        {
          Log::log( NS_LOG_HEADER + "item without entity",
                    LOG_LEVEL_ERROR );
          return;
        }
      }

      // Publish selection
      std::vector< unsigned int > ids;
      SelectionManager::selectableEntitiesIds( ids );
      ZeroEQManager::publishSelection( ids );

    } // selection event
  }

  void InteractionManager::_propagateSelectedStateToChilds(
    shift::Entities& entities,
    shift::RelationshipOneToN& relParentOf,
    const shift::RelationshipOneToN& relSuperEntityOf,
    unsigned int entityGid,
    SelectedState state )
  {
    const auto& childrenIds = relParentOf[ entityGid ];
    std::cout << " -- Parent of: ";
    for ( auto const& childId : childrenIds )
    {
      std::cout << childId << " ";
      if ( relSuperEntityOf.count( childId ) > 0 )
      {
        const auto& subEntities = relSuperEntityOf.at( childId );
        for ( const auto& subEntity : subEntities )
          SelectionManager::setSelectedState(
            DataManager::entities( ).at( subEntity ), state );
      }
      SelectionManager::setSelectedState(
        entities.at( childId ), state );
      _propagateSelectedStateToChilds( entities, relParentOf, relSuperEntityOf,
                                       childId, state );
    }

  }

  void InteractionManager::_propagateSelectedStateToParent(
    shift::Entities& entities,
    shift::RelationshipOneToOne& relChildOf,
    shift::RelationshipOneToN& relParentOf,
    unsigned int entityGid,
    SelectedState childState )
  {
    const auto& parentId = relChildOf[ entityGid ];

    if ( parentId == 0 ) return;

    if ( childState == SelectedState::PARTIALLY_SELECTED )
    {
      //std::cout << "<>Partially selected" << std::endl;
      SelectionManager::setSelectedState(
        entities.at( parentId ), childState );
      _propagateSelectedStateToParent( entities, relChildOf, relParentOf,
                                       parentId, childState );
      return;
    }

    bool allChildrenSelected, noChildrenSelected;
    queryChildrenSelectedState( entities, relParentOf, parentId,
                                allChildrenSelected, noChildrenSelected );
    //std::cout << "<>AllChildSelected? = " << allChildrenSelected << std::endl;
    SelectedState state;
    if ( noChildrenSelected )
      state = SelectedState::UNSELECTED;
    else if ( allChildrenSelected )
      state = SelectedState::SELECTED;
    else
      state = SelectedState::PARTIALLY_SELECTED;

    SelectionManager::setSelectedState(
      entities.at( parentId ), state );
    _propagateSelectedStateToParent( entities, relChildOf, relParentOf,
                                     parentId, state );
  }

  void InteractionManager::queryChildrenSelectedState(
    const shift::Entities& entities,
    shift::RelationshipOneToN& relParentOf,
    unsigned int entityGid,
    bool& allChildrenSelected,
    bool& noChildrenSelected )
  {
    allChildrenSelected = true;
    noChildrenSelected = true;
    const auto& childrenIds = relParentOf[ entityGid ];
    for ( auto const& childId : childrenIds )
    {
      if ( SelectionManager::getSelectedState( entities.at( childId )) !=
           SelectedState::SELECTED )
        allChildrenSelected = false;
      if ( SelectionManager::getSelectedState( entities.at( childId )) ==
           SelectedState::SELECTED )
        noChildrenSelected = false;
    }
    return;
  }
}
