
/**
 * @file MongoMapReduceAtom.cpp
 * @author Chao ZHANG
 * @date 04/22/2014 21:09:10
 *
 * @brief Defintion of the Mongo Atom.
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "MongoMapReduceAtom.h"

#include "ConfigFile.h"

#include <dlvhex2/Registry.h>
#include <dlvhex2/Term.h>

#include "mongo/client/dbclient.h"
#include <sstream>
#include <string>
#include <iostream>
#include <vector>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>

using namespace std;
using namespace mongo;

namespace dlvhex {
	namespace mongo1 {

		MongoMapReduceAtom::MongoMapReduceAtom() : PluginAtom("mongo_mapreduce", 1) {

			addInputTuple();
			setOutputArity(1);
		}

		void
			MongoMapReduceAtom::retrieve(const Query& query, Answer& answer) throw (PluginError) {
				Registry &registry = *getRegistry();

				mongo::DBClientConnection c;
				ConfigFile cfg("include/config.cfg");
				std::string connection = cfg.getValueOfKey("CONNECTION_STRING");


				// try the connection of the mongo db at first               
				try {

					c.connect(connection);

				} catch (const mongo::DBException &e) {
					throw PluginError("MongoDB is not on, or connection breaks!");
				}

				int arity = query.input.size();

				if(arity < 3 || arity > 4) {

					throw PluginError("The input tuple should have 3 or 4 parameters: \"db.collection\", \"mapper function\", \"reducer function\", and optional \"query operator\".");

				}

				std::string s_dbc = registry.terms.getByID(query.input[0]).getUnquotedString();
				std::string s_mapper = registry.terms.getByID(query.input[1]).getUnquotedString();
				std::string s_reducer = registry.terms.getByID(query.input[2]).getUnquotedString();
				std::string s_query;

				if(arity == 4) {            
					s_query =  registry.terms.getByID(query.input[3]).getUnquotedString();
				}

				else {
					s_query = "{}";

				}


				if(s_mapper.substr(0,8) != "function" || s_reducer.substr(0,8) != "function") {

					throw PluginError("The map and reduce function should start with \"function() \"");

				}

				c.mapreduce(s_dbc, s_mapper, s_reducer, fromjson(s_query), "mapreduce_result");   


				// The ouput projection items shuld be seperated by ; 
				std::vector<string> db_collection;
				boost::split(db_collection, s_dbc, boost::is_any_of("."));

				// Start up a cursor to run a tranverse search for the data
				std::auto_ptr<DBClientCursor> cursor = c.query(db_collection[0] + ".mapreduce_result", fromjson(s_query));

				if (!cursor.get()) {

					throw PluginError("query failure");

				}

				while (cursor->more()) {

					Tuple out;

					BSONObj p = cursor->next();


					std::string in;

					in = p.toString();

					boost::replace_all(in, "\"", "'");
					if (in.substr(0, 1) == "'" && in.substr(in.size() - 1, 1) == "'") {

						boost::replace_all(in, "'", "");
					}

					Term term(ID::MAINKIND_TERM | ID::SUBKIND_TERM_CONSTANT, '"' + in + '"');
					out.push_back(registry.storeTerm(term));

					answer.get().push_back(out);
				}// end of while

			} // end of retrieve function

	} // end of mongo
} // end of dlvhex







