#ifndef sort_with_temp_files_h
#define sort_with_temp_files_h

#include  "bserialize.h"


const size_t SortBufferSize = 100000000;
//const size_t SortBufferSize = 10;


template <class T>
bool CopyRestOfBinaryFiles (FILE* in_fp, FILE* out_fp, size_t i, size_t Max_i, T& V)
{
    for (; i < Max_i; )
    {
        if (!BinaryWriteItem(out_fp, V))
            return false;
        i++;
        if (i != Max_i)
            if (!BinaryReadItem(in_fp, V))
                return false;
    }
    return true;

}

// produces not-unique sequence 
template <class T>
string UnionBinaryFiles (string InputFileName1, string InputFileName2)
{
    string OutputFileName = CreateTempFileName();
    FILE* fp1 = 0;
    FILE* fp2 = 0;
    FILE* out_fp = 0;
    T V1, V2;
    size_t Count1 = (size_t)FileSize(InputFileName1.c_str())/get_size_in_bytes(V1);
    size_t Count2 = (size_t)FileSize(InputFileName2.c_str())/get_size_in_bytes(V2);
    
    try {
        if (Count1 == 0)
            if (!RmlMoveFile (InputFileName2.c_str(), OutputFileName.c_str()))
                throw (Format( "Could not move %s to %s", InputFileName2.c_str(), OutputFileName.c_str()));
            else
                return OutputFileName;
        if (Count2 == 0)
            if (!RmlMoveFile (InputFileName1.c_str(), OutputFileName.c_str()))
                throw (Format( "Could not move %s to %s", InputFileName1.c_str(), OutputFileName.c_str()));
            else
                return OutputFileName;

    

        fp1 = fopen (InputFileName1.c_str(), "rb");
        if (!fp1)
            throw CExpc ("cannot read %s\n", InputFileName1.c_str());
        fp2 = fopen (InputFileName2.c_str(), "rb");
        if (!fp2)
            throw CExpc ("cannot read %s\n", InputFileName2.c_str());
        
        
        if (!BinaryReadItem(fp1, V1))
            throw CExpc ("cannot read item form %s\n", InputFileName1.c_str());

        if (!BinaryReadItem(fp2, V2))
            throw CExpc ("cannot read item form %s\n", InputFileName2.c_str());

        size_t i1 = 0;
        size_t i2 = 0;
        string Error; 
        out_fp = fopen (OutputFileName.c_str(), "wb");

 	    for (; i1 != Count1 && i2 != Count2; )
        {
		    if ( V1 < V2 )
            {
                if (!BinaryWriteItem(out_fp, V1))
                    throw CExpc ("cannot write item to %s\n", OutputFileName.c_str());

                i1++;
                if (i1 != Count1)
                    if (!BinaryReadItem(fp1, V1))
                        throw CExpc ("cannot read item form %s\n", InputFileName1.c_str());
            }
		    else if (V2 < V1)
            {
                if (!BinaryWriteItem(out_fp, V2))
                    throw CExpc ("cannot write item to %s\n", OutputFileName.c_str());

                i2++;
                if (i2 != Count2)
                    if (!BinaryReadItem(fp2, V2))
                        throw CExpc ("cannot read item form %s\n", InputFileName2.c_str());
            }
		    else
            {

                if (!BinaryWriteItem(out_fp, V1))
                    throw CExpc ("cannot write item to %s\n", OutputFileName.c_str());

                if (!BinaryWriteItem(out_fp, V2))
                    throw CExpc ("cannot write item to %s\n", OutputFileName.c_str());

                i1++;
                if (i1 != Count1)
                    if (!BinaryReadItem(fp1, V1))
                        throw CExpc ("cannot read item form %s\n", InputFileName1.c_str());

                i2++;
                if (i2 != Count2)
                    if (!BinaryReadItem(fp2, V2))
                        throw CExpc ("cannot read item form %s\n", InputFileName2.c_str());
            }
        }
        if (!CopyRestOfBinaryFiles<T>(fp1, out_fp, i1, Count1, V1) ||  !CopyRestOfBinaryFiles<T>(fp2, out_fp, i2, Count2, V2))
            throw CExpc("unexpected error in CopyBinaryFiles");
    }
    catch ( CExpc c)
    {
        fprintf (stderr, "Error in UnionBinaryFiles: %s", c.m_strCause.c_str());
        remove (OutputFileName.c_str());
        OutputFileName = "";
    }

    if (fp1) fclose (fp1);
    if (fp2) fclose (fp2);
    if (out_fp) fclose (out_fp);

    return OutputFileName;

}

template <class T>
bool SortWithFiles (string InputFileName, string OutputFileName)
{
    file_off_t sz = FileSize(InputFileName.c_str());
    T dummy;
    size_t Count = (size_t)sz/get_size_in_bytes(dummy);
    size_t PortionSize = max((size_t)1, SortBufferSize/get_size_in_bytes(dummy)); // 100 Mb for sorting buffer
    
    FILE* fp = fopen (InputFileName.c_str(), "rb");
    if (!fp)
    {   
         fprintf (stderr, "Cannot open %s", InputFileName.c_str());
         return false;
    }
    vector<string> PortionNames;
    for (size_t i =0; i<Count; i+= PortionSize)
    {
        size_t End = min (Count, i+PortionSize);
        vector<T> Portion;
        ReadVectorInner(fp, Portion, End - i);
        sort (Portion.begin(),  Portion.end());
        PortionNames.push_back(CreateTempFileName());
        WriteVector(PortionNames.back(), Portion);
    }
    fclose (fp);
    
    while (PortionNames.size() > 1)
    {
        size_t  SaveCount = PortionNames.size();
        vector<string> NewPortionNames;
        for (size_t i=0; i < PortionNames.size(); i+=2)
        {
            if (i+1 == PortionNames.size())
                NewPortionNames.insert(NewPortionNames.begin(), PortionNames.back()); // insert to the beginning
            else
            {
                string UnitedFile = UnionBinaryFiles<T>(PortionNames[i], PortionNames[i+1]);
                if (UnitedFile.empty())
                    return false;
                
                NewPortionNames.push_back( UnitedFile  );
                if (remove(PortionNames[i+1].c_str()) != 0)
                {
                    fprintf (stderr, "Cannot remove temp file%s", PortionNames[i+1].c_str());
                    return false;
                }
                if (remove(PortionNames[i].c_str()) !=0 )
                {
                    fprintf (stderr, "Cannot remove temp file%s", PortionNames[i].c_str());
                    return false;
                }
            }
        }
        NewPortionNames.swap(PortionNames);
        if (PortionNames.size() == SaveCount)
            throw CExpc("dead lock in SortWithFiles");
        SaveCount = PortionNames.size();
    }
    if (FileExists(OutputFileName.c_str()))
        remove (OutputFileName.c_str());
    return RmlMoveFile (PortionNames[0].c_str(), OutputFileName.c_str());
    

}

template <class T>
bool SortWithFiles (string InputFileName)
{
    string TmpName = CreateTempFileName();
    if (!SortWithFiles<T>(InputFileName, TmpName))
               return false;

    remove (InputFileName.c_str());

    if (!RmlMoveFile (TmpName.c_str(), InputFileName.c_str()))
    {
        fprintf (stderr, "cannot move %s to %s\n",TmpName.c_str(), InputFileName.c_str());
        remove (TmpName.c_str());
        return false;
    }
    return true;
}


#endif
