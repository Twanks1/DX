#include "config_file.h"
/**********************************************************************
    class: ConfigFile (config_file.cpp)

    author: S. Hau
    date: October 18, 2017

    @Considerations:
      - Overthink how to write data to disk, because of:
         1.) Lines which could not be read will be removed on flush
         2.) Why flush when only one line has changed? (or even nothing)
           -> Write only lines which has changed?
         3.) Integrate a order for categories
**********************************************************************/

#include "Logging/logging.h"
#include "Common/string_utils.h"
#include "OS/FileSystem/file.h"

namespace Core { namespace Config {

    //----------------------------------------------------------------------
    const char* ConfigFile::DEFAULT_CATEGORY_NAME = "Default";

    //----------------------------------------------------------------------
    ConfigFile::ConfigFile( const char* vpath, bool flushOnDeconstruct ) 
        : m_flushOnDeconstruct( flushOnDeconstruct )
    {
        try
        {
            m_configFile = new OS::BinaryFile( vpath, OS::EFileMode::READ_WRITE );
            _Read();
        }
        catch (const std::runtime_error&)
        {
            LOG_WARN( "ConfigFile: File " + String( vpath ) + " does not exist." );
        }
    }

    //----------------------------------------------------------------------
    ConfigFile::~ConfigFile()
    {
        if (m_flushOnDeconstruct)
            flush();
        delete m_configFile;
    }

    //**********************************************************************
    // PUBLIC 
    //**********************************************************************

    //----------------------------------------------------------------------
    void ConfigFile::flush()
    {
        m_configFile->clear();

        for (auto& categoryPair : m_categories)
        {
            const char* categoryName = categoryPair.first.c_str();

            // Don't write default category name 
            if (categoryPair.first != SID(DEFAULT_CATEGORY_NAME))
                m_configFile->write( "[%s]\n", categoryName );

            // Write all data line by line based on their datatypes
            for (auto& linePair : categoryPair.second.m_entries)
            {
                const char* name = linePair.first.c_str();
                m_configFile->write( "%s = ", name );

                Common::VariantType& value = linePair.second;
                switch ( value.getType() )
                {
                case Common::EVariantType::BOOL:    *m_configFile << value.get<bool>(); break;
                case Common::EVariantType::I32:     *m_configFile << value.get<I32>(); break;
                case Common::EVariantType::U32:     *m_configFile << value.get<U32>(); break;
                case Common::EVariantType::I64:     *m_configFile << value.get<I64>(); break;
                case Common::EVariantType::U64:     *m_configFile << value.get<U64>(); break;
                case Common::EVariantType::F32:     *m_configFile << value.get<F32>(); break;
                case Common::EVariantType::F64:     *m_configFile << value.get<F64>(); break;
                case Common::EVariantType::String:  *m_configFile << value.get<const char*>(); break;
                }
                m_configFile->write( "\n" );
            }
            m_configFile->write( "\n" );
        }

        m_configFile->flush();
    }

    //**********************************************************************
    // PRIVATE 
    //**********************************************************************

    //----------------------------------------------------------------------
    void ConfigFile::_Read()
    {
        StringID currentCategory = SID( DEFAULT_CATEGORY_NAME );
        m_categories[currentCategory] = Category();

        while ( not m_configFile->eof() )
        {
            String line = m_configFile->readLine();

            // Skip empty lines
            if (line.size() == 0)
                continue;

            // Check if line is a category ("[Category]")
            String categoryName = StringUtils::substringBetween( line, '[', ']' );
            StringUtils::trim( categoryName );
            if ( categoryName.size() > 0 )
            {
                // Add new category
                currentCategory = SID( categoryName.c_str() );
                continue;
            }

            // Try to read line as a name-value pair (e.g. "life=42")
            auto strings = StringUtils::splitString( line, '=' );
            if (strings.size() == 2)
            {
                const String& valueName = strings[0];
                const String& value     = strings[1];

                // Add value to current category
                m_categories[currentCategory][valueName.c_str()] = value;

                continue;
            }

            LOG_WARN( "ConfigFile '" + m_configFile->getFilePath().toString() + "': Could not read line: '" + line + "'" );
        }
    }

    //**********************************************************************
    // CATEGORY 
    //**********************************************************************


} }