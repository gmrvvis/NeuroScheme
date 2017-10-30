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
#ifndef __NSPLUGINS_CONGEN__REPRESENTATION_CREATOR__
#define __NSPLUGINS_CONGEN__REPRESENTATION_CREATOR__
#include <shift/shift.h>
#include <nslib/mappers/VariableMapper.h>
#include <scoop/scoop.h>

#include <unordered_map>
#include <set>

namespace nslib
{
  namespace congen
  {

    class RepresentationCreator : public shift::RepresentationCreator
    {
    public:

      RepresentationCreator( void );

      virtual ~RepresentationCreator( void ) {};

      void create(
        const shift::Entities& entities,
        shift::Representations& representations,
        shift::TEntitiesToReps& entitiesToReps,
        shift::TRepsToEntities& repsToEntities,
        shift::TGidToEntitiesReps& gidsToEntitiesReps,
        bool linkEntitiesToReps = false,
        bool linkRepsToObjs = false ) final;

      void generateRelations(
        const shift::Entities& entities,
        const shift::TGidToEntitiesReps& gidsToEntitiesReps,
        shift::TRelatedEntitiesReps& relatedEntitiesReps,
        shift::Representations& relatedEntities,
        const std::string& relationName );

#define TripleKey( x, y, z ) std::make_pair( x, std::make_pair( y, z ))

      bool entityUpdatedOrCreated( shift::Entity* entity ) final;
      bool relationshipUpdatedOrCreated(
        shift::RelationshipProperties* relProperties ) final;

    protected:

      unsigned int _maxNeuronsPerPopulation;
      float _maxAbsoluteWeight;
};

  } // namespace congen
} // namespace nslib

#endif // __NSLIB__REPRESENTATION_CREATOR__