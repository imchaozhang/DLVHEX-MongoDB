
/**
 * @file MongoQueryAtom.cpp
 * @author Chao ZHANG
 * @date 04/22/2014 21:09:10
 *
 * @brief Defintion of the Mongo Atom.
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "MongoQueryAtom.h"

#include "ConfigFile.h"

#include <dlvhex2/Registry.h>
#include <dlvhex2/Term.h>

#include "mongo/client/dbclient.h"
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <stdlib.h> 

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>

using namespace std;
using namespace mongo;

namespace dlvhex {
    namespace mongo1 {

        MongoQueryAtom::MongoQueryAtom(int& i) : PluginAtom("mongo_query"+boost::lexical_cast<std::string>(i), 1) {

	    addInputTuple();
            setOutputArity(i);
        }

        MongoQueryAtom::~MongoQueryAtom()
        {
        
        }
        void
        MongoQueryAtom::retrieve(const Query& query, Answer& answer) throw (PluginError) {
            Registry &registry = *getRegistry();

            mongo::DBClientConnection c;
            ConfigFile cfg("/data/dlvhex-config/config.cfg");
            std::string connection = cfg.getValueOfKey("CONNECTION_STRING");


            // try the connection of the mongo db at first               
            try {

                c.connect(connection);

            } catch (const mongo::DBException &e) {
                throw PluginError("MongoDB is not on, or connection breaks!");
            }
            
            int arity = query.input.size();
            
            if(arity < 3) {
            
            	throw PluginError("The input tuple should have 3 parameters at least");
            
            }
			
            if(arity > 13) {
            
            	throw PluginError("Cannot have input parameters more than 10");
            
            }


            const Term& dbc = registry.terms.getByID(query.input[0]);
            const Term& jpattern = registry.terms.getByID(query.input[1]);
            const Term& output = registry.terms.getByID(query.input[2]);



            if (!dbc.isQuotedString()) {
                throw PluginError("Wrong input argument type for input 1, need to be string");
            }


            if (!jpattern.isQuotedString()) {
                throw PluginError("Wrong input argument type for input 2, need to be string");
            }

            if (!output.isQuotedString()) {
                throw PluginError("Wrong input argument type for input 3, need to be string");
            }


            std::string s_dbc = dbc.getUnquotedString();
            std::string s_jpattern = jpattern.getUnquotedString();
            std::string s_output = output.getUnquotedString();

			// Add INPUT parameter into a stream array, the maximum size of the array will be 10
			
			std::string value_input_array[10];
			
	    for (int i = 3; i < arity; i++) {
	    
	    	ID id_input = query.input[i];
	    	std::stringstream value_input;
	    	
	    	
                if (id_input.isConstantTerm()) {

                    const Term &term = registry.terms.getByID(id_input);
                    value_input << term.getUnquotedString();

                } else if (id_input.isIntegerTerm()) {

                    value_input << id_input.address;

                }
                
               else {
               
               	throw PluginError("Wrong input argument type for input" + (i+1));
               
               }
			   
			   value_input_array[i-3] = value_input.str();
 
   	    }// end for
            
			
			   // find the index of $INPUT	   
               
               if(arity>3 && arity < 14 && s_jpattern.find("$INPUT_") == -1) {
               
               	throw PluginError("The query pattern needs to have \"$INPUT_N\" to represent the input element; N = 0 ... 9");
                   
               }
			   
               else {
				   
				   int i = 0;
				   
				   while(i < s_jpattern.length() && s_jpattern.find("$INPUT_") != -1){
				
				   i = static_cast<int>(s_jpattern.find("$INPUT_"));
				   
				   std::stringstream stream_temp;
				   
				   stream_temp << s_jpattern[i+7];
				   
				   int s_jpattern_index; 
				   
				   stream_temp >> s_jpattern_index;
				   
				   
				   s_jpattern.replace(s_jpattern.find("$INPUT_"),8,value_input_array[s_jpattern_index]);			   
                
               }//end of while loop
			   
		   }//end of if-else



            if (s_output == "") {

                throw PluginError("The output pattern cannot be empty");

            }

            // The ouput projection items shuld be seperated by ; 
            std::vector<string> output_v;
            boost::split(output_v, s_output, boost::is_any_of(";"));

            // Start up a cursor to run a tranverse search for the data
            std::auto_ptr<DBClientCursor> cursor = c.query(s_dbc, fromjson(s_jpattern));

            if (!cursor.get()) {

                throw PluginError("query failure");

            }

            while (cursor->more()) {

                Tuple out;

                BSONObj p = cursor->next();

                if (s_output == "{}" || s_output == "$ALL") {
                
		        std::string in;
		        
		        in = p.toString();
		        
		        boost::replace_all(in, "\"", "'");
		                        if (in.substr(0, 1) == "'" && in.substr(in.size() - 1, 1) == "'") {

		                            boost::replace_all(in, "'", "");
		                        }

		        Term term(ID::MAINKIND_TERM | ID::SUBKIND_TERM_CONSTANT, '"' + in + '"');
		                    out.push_back(registry.storeTerm(term));


                } else {

                    for (size_t i = 0; i < output_v.size(); i++) {

                        std::string in;

                        try {

                            in = p.getFieldDotted(output_v[i]).String();

                        } catch (DBException &e) {

                            if (p.getFieldDotted(output_v[i]).isNumber()) {

                                ostringstream convert;
                                convert << p.getFieldDotted(output_v[i]).toString(false);
                                in = convert.str();
                                // if it is int, remove the last two digits
                                if (in.size() >= 3 && in.substr(in.size() - 2, 2) == ".0")
                                    in = in.substr(0, in.size() - 2);

                            } else if (p.getFieldDotted(output_v[i]).isBoolean()) {
                                ostringstream convert;

                                if (p.getFieldDotted(output_v[i]).boolean() == true)
                                    in = "true";
                                else
                                    in = "false";

                            }// Check if the element type is Date
                            else if (p.getFieldDotted(output_v[i]).type() == 9) {

                                ostringstream convert;

                                convert << p.getFieldDotted(output_v[i]).date();


                                in = convert.str();

                                if (in.size() == 13)
                                    // remove millionsecond to the epoch time stamp
                                    in = in.substr(0, 10);


                            } else if (!p.getFieldDotted(output_v[i]).eoo()) {


                                in = p.getFieldDotted(output_v[i]).toString(false);

                                //in.erase(
                                //	remove(in.begin(), in.end(), '\"'),
                                // 	in.end()
                                //        );    
                                boost::replace_all(in, "\"", "'");
                                if (in.substr(0, 1) == "'" && in.substr(in.size() - 1, 1) == "'") {

                                    boost::replace_all(in, "'", "");
                                }

                            } else {

                                in = "";

                            }

                        } // end of try - catch

                        //if(!p.hasField(output_v[i]))
                        // {
                        //	cout<< "Some of the document do not have field \"" << output_v[i] << "\"" << endl;
                        //}

                        try {
                            int v;
                            v = boost::lexical_cast<int>(in);
                            Term term(ID::MAINKIND_TERM | ID::SUBKIND_TERM_CONSTANT, in);
                            out.push_back(registry.storeTerm(term));

                        } catch (boost::bad_lexical_cast& e) {

                            Term term(ID::MAINKIND_TERM | ID::SUBKIND_TERM_CONSTANT, '"' + in + '"');
                            out.push_back(registry.storeTerm(term));
                        }
                    } // end of for
                } // end of if - else
                answer.get().push_back(out);
            }// end of while



        } // end of retrieve function

    } // end of mongo
} // end of dlvhex
