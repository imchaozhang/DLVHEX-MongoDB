/* -*- C++ -*- */
/**
 * @file MongoPlugin.h
 * @author Chao ZHANG
 * @date 04/22/2014 21:09:10 *
 * @brief Header file for MongoPlugin
 *
 */
#ifndef _MONGO_PLUGIN_H
#define _MONGO_PLUGIN_H

#include <dlvhex2/PluginInterface.h>
#include <dlvhex2/Error.h>



namespace dlvhex {
  namespace mongo1 {


class MongoPlugin : public PluginInterface
{

public:
  MongoPlugin();

  //adding by Chao
  std::vector<PluginAtomPtr> createAtoms(ProgramCtx&) const;
    
};  // class MongoPlugin



  } // namespace mongo
} // namespace dlvhex


#endif /* _MONGO_PLUGIN_H */
