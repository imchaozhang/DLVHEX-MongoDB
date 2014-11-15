/* -*- C++ -*- */
/**
 * @file MongoPlugin.cpp
 * @author Chao ZHANG
 * @date 04/22/2014 21:09:10
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "MongoPlugin.h"
#include "MongoQueryAtom.h"
#include "MongoInsertAtom.h"
#include "MongoArrayAtom.h"
#include "MongoJSONAtom.h"
#include "MongoMapReduceAtom.h"
#include <dlvhex2/PluginInterface.h>
#include <dlvhex2/Logger.h>

#include <iostream>
#include <sstream>


namespace dlvhex {
  namespace mongo1 {


  MongoPlugin::MongoPlugin()

  {
    setNameVersion(PACKAGE_TARNAME,MONGOPLUGIN_VERSION_MAJOR,MONGOPLUGIN_VERSION_MINOR,MONGOPLUGIN_VERSION_MICRO);
  }


  
 std::vector<PluginAtomPtr> MongoPlugin::createAtoms(ProgramCtx&) const
      {
          std::vector<PluginAtomPtr> ret;
          
          int max_index = 20;
          for(int i=1;i<=max_index;i++){
          ret.push_back(PluginAtomPtr(new MongoQueryAtom(i), PluginPtrDeleter<PluginAtom>()));
          ret.push_back(PluginAtomPtr(new MongoJSONAtom(i), PluginPtrDeleter<PluginAtom>()));
          }
          
          ret.push_back(PluginAtomPtr(new MongoInsertAtom, PluginPtrDeleter<PluginAtom>()));
          ret.push_back(PluginAtomPtr(new MongoArrayAtom, PluginPtrDeleter<PluginAtom>()));

          ret.push_back(PluginAtomPtr(new MongoMapReduceAtom, PluginPtrDeleter<PluginAtom>()));          

          
          return ret;
      }
      
MongoPlugin theMongoPlugin;


  } // namespace mongo
} // namespace dlvhex

//
// let it be loaded by dlvhex!
//

IMPLEMENT_PLUGINABIVERSIONFUNCTION

// return plain C type s.t. all compilers and linkers will like this code
extern "C"
void * PLUGINIMPORTFUNCTION()
{
	return reinterpret_cast<void*>(& dlvhex::mongo1::theMongoPlugin);
}

