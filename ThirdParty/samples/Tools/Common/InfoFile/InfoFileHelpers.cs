using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.Xml.Schema;

namespace AK.Wwise.InfoFile
{
    class InfoFileHelpers
    {
        const string INFO_SCHEMA_VERSION = "10";

        /// <summary>
        /// Load an soundbanks info file. Its data is returned in a SoundBanksInfo class.
        /// </summary>
        /// <param name="file">INFO file to be loaded</param>
        static internal AK.Wwise.InfoFile.SoundBanksInfo LoadInfoFile(string in_file)
        {
            XmlReaderSettings readerSettings = new XmlReaderSettings();
            readerSettings.ValidationType = ValidationType.Schema;
            string szSchemaFolder = System.IO.Path.GetDirectoryName(System.Windows.Forms.Application.ExecutablePath);
            string szSchemaFile = System.IO.Path.Combine(szSchemaFolder, @"..\..\..\..\Data\Schemas\SoundbanksInfo.xsd");
            readerSettings.Schemas.Add("", szSchemaFile);

            //Check the schema version before loading the entire file.
            try
            {
                using (XmlReader reader = XmlReader.Create(in_file, readerSettings))
                {
                    // Verify the schema version.
                    reader.MoveToContent();
                    if (reader.GetAttribute("SchemaVersion") != INFO_SCHEMA_VERSION)
                    {
                        throw new Exception("Wrong Info file schema version. Please check the version of Wwise used to generate your SoundBanks.");
                    }
                }
            }
            catch( Exception e )
            {
                throw new Exception("\n\nWrong Info file schema version. Please check the version of Wwise used to generate your SoundBanks. \n\n(" + e.Message + ")" );
            }

            AK.Wwise.InfoFile.SoundBanksInfo data = AK.Wwise.InfoFile.SoundBanksInfo.LoadFromFile(in_file);
            
            return data;
        }
    }

    public partial class FileDescriptorType : IEquatable<FileDescriptorType>
    {
        public virtual bool Equals(  FileDescriptorType obj )
        { 
            return this.Id == obj.Id && this.Language == obj.Language; 
        }
        public override int GetHashCode()
        {
            return Tuple.Create(Id, Language).GetHashCode();
        }
    }
}
