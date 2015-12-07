#include "JSONWrapper.h"

using namespace Reymenta;

JSONWrapper::JSONWrapper() 
{

	CI_LOG_V("JSONWrapper constructor");
}

JSONWrapper::~JSONWrapper() 
{
	CI_LOG_V("JSONWrapper destructor");
}


string JSONWrapper::parseUIJson()
{
	// params
	stringstream params;
	params << "{ \"ok\":1";
	string pathToFile = ( getAssetPath("") / "json" / "ui.json" ).string();
	//dual screen string pathToFile = ( getAssetPath("") / "json" / "config.json" ).string();
	try 
	{
		if ( !fs::exists( pathToFile ) ) 
		{
			CI_LOG_V( "file not found: " +  pathToFile ); 
			return "{ \"ok\":0 }";
		}
		else
		{
			CI_LOG_V( "found file: " +  pathToFile ); 
		}
		JsonTree json;
		try
		{
			json = JsonTree( loadFile( pathToFile ) );
		} 
		catch(cinder::JsonTree::Exception exception) 
		{ 
			CI_LOG_V("jsonparser exception " + pathToFile +": " + exception.what());
			return  "{ \"ok\":0 }";
		}
		JsonTree jsonParams = json.getChild( "params" );   
		for( JsonTree::ConstIter jsonElement = jsonParams.begin(); jsonElement != jsonParams.end(); ++jsonElement ) 
		{			
			string name = jsonElement->getChild( "name" ).getValue<string>();
			string value = jsonElement->getChild( "value" ).getValue<string>();
			params << ", \"";
			params << name;
			params << "\":\"";
			params << value;
			params << "\"";
		} 
	} 
	catch ( ... ) 
	{
		CI_LOG_V(  "Fragjson parsing error: " +  pathToFile );
		return  "{ \"ok\":0 }";
	}
	params << "}";

	return params.str();
}
bool JSONWrapper::parseAssetsJson( TexturesRef aTexturesRef )
{
	mTextures = aTexturesRef;

	string pathToFile = ( getAssetPath("") / "json" / "assets.json" ).string();
	try 
	{
		if ( !fs::exists( pathToFile ) ) 
		{
			CI_LOG_V( "file not found: " +  pathToFile ); 
			return false;
		}
		else
		{
			CI_LOG_V( "found file: " +  pathToFile ); 
		}
		JsonTree json;
		try
		{
			json = JsonTree( loadFile( pathToFile ) );
		} 
		catch(cinder::JsonTree::Exception exception) 
		{ 
			CI_LOG_V("jsonparser exception " + pathToFile +": " + exception.what());
			return false;
		}

		//Assets
		int i = 1; // 0 is audio
		JsonTree jsons = json.getChild( "assets" );   
		for( JsonTree::ConstIter jsonElement = jsons.begin(); jsonElement != jsons.end(); ++jsonElement ) 
		{			
			if ( i < mTextures->getTextureCount() )
			{
				string jsonFileName = jsonElement->getChild( "filename" ).getValue<string>();
				CI_LOG_V("asset filename: " + jsonFileName );
				mTextures->setTexture( i, jsonFileName );

				i++;
			}
		} 
	} 
	catch ( ... ) 
	{
		CI_LOG_V(  "Parsing error: " +  pathToFile );
		return false;
	}
	return true;
}
