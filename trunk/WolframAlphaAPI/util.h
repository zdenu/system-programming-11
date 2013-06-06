//
//  util.h
//  WolframAlphaAPI
//
//  Created by Christopher Kim on 6/4/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//

#pragma once

#include "Define.h"
#include "giflib/gif_lib.h"
#include "giflib/gif_lib_private.h"

namespace Util
{
	void dumpMemory(void *data, int dataSize )
    {
        printf("## dump memory size:%d address:%p\n", dataSize, data);
        
        if( NULL == data )
            return;
        
        int i = 0;
        int32_t length = dataSize;
        char *address = (char*)data;
        char *line = (char*)address;
        unsigned char ch;
        
        int dumpDataSize = length * 16;
        char* dumpData = new char[dumpDataSize];
        char temp[256];
        snprintf( dumpData, dumpDataSize, ";\n0x%08llX | ", (int64_t)address);
        
        while (length-- > 0) {
            
            snprintf( temp, sizeof(temp), "%02X ", (unsigned char)*address++);
            strcat( dumpData, temp );
            
            if( (i + 1)% 8 == 0 && (i + 1)%16 != 0 )
            {
                snprintf( temp, sizeof(temp), "  " );
                strcat( dumpData, temp );
            }
            
            if (!(++i % 16) || (length == 0 && i % 16))
            {
                if (length == 0)
                {
                    while (i++ % 16)
                    {
                        snprintf( temp, sizeof(temp), "   ");
                        strcat( dumpData, temp );
                        if( (i + 1)% 8 == 0 && (i + 1)%16 != 0 )
                        {
                            snprintf( temp, sizeof(temp), "  " );
                            strcat( dumpData, temp );
                        }
                    }
                }
                
                snprintf( temp, sizeof(temp), "|");
                strcat( dumpData, temp );
                while (line < address)
                {
                    ch = *line++;
                    snprintf( temp, sizeof(temp), "%c", (ch < 33 || ch == 255) ? 0x2E : ch);
                    strcat( dumpData, temp );
                }
                
                if (length > 0)
                {
                    snprintf( temp, sizeof(temp), "\n0x%08llX | ", (int64_t)address);
                    strcat( dumpData, temp );
                }
            }
        }
        
        snprintf( temp, sizeof(temp), "\n");
        strcat( dumpData, temp );
        
		printf("%s\n", dumpData);
		
        delete [] dumpData;
    }
	
	
	
	bool GifNoisyPrint = true;
	
	/*****************************************************************************
	 Same as fprintf to stderr but with optional print.
	 ******************************************************************************/
	void
	GifQprintf(char *Format, ...) {
		char Line[128];
		va_list ArgPtr;
		
		va_start(ArgPtr, Format);
		
		if (GifNoisyPrint) {
			(void)vsnprintf(Line, sizeof(Line), Format, ArgPtr);
			(void)fputs(Line, stderr);
		}
		
		va_end(ArgPtr);
	}
	
	void
	PrintGifError(int ErrorCode) {
		char *Err = GifErrorString(ErrorCode);
		
		if (Err != NULL)
			fprintf(stderr, "GIF-LIB error: %s.\n", Err);
		else
			fprintf(stderr, "GIF-LIB undefined error %d.\n", ErrorCode);
	}
	
	static void DumpScreen2RGB(char *FileName, int OneFileFlag,
							   ColorMapObject *ColorMap,
							   GifRowType *ScreenBuffer,
							   int ScreenWidth, int ScreenHeight)
	{
		int i, j;
		GifRowType GifRow;
		GifColorType *ColorMapEntry;
		FILE *f[3];
		
		if (FileName != NULL) {
			char OneFileName[80];
			
			if (OneFileFlag) {
				if ((f[0] = fopen(FileName, "wb")) == NULL)
					return;
				//				GIF_EXIT("Can't open input file name.");
			} else {
				static char *Postfixes[] = { ".R", ".G", ".B" };
				
				for (i = 0; i < 3; i++) {
					strncpy(OneFileName, FileName, sizeof(OneFileName)-1);
					/* cppcheck-suppress uninitstring */
					strncat(OneFileName, Postfixes[i],
							sizeof(OneFileName) - 1 - strlen(OneFileName));
					
					if ((f[i] = fopen(OneFileName, "wb")) == NULL) {
						return;
						//                    GIF_EXIT("Can't open input file name.");
					}
				}
			}
		} else {
			OneFileFlag = true;
			
#ifdef _WIN32
			_setmode(1, O_BINARY);
#endif /* _WIN32 */
			
			f[0] = stdout;
		}
		
		if (OneFileFlag) {
			unsigned char *Buffer, *BufferP;
			
			if ((Buffer = (unsigned char *) malloc(ScreenWidth)) == NULL)
				return ;
			//            GIF_EXIT("Failed to allocate memory required, aborted.");
			for (i = 0; i < ScreenHeight; i++) {
				GifRow = ScreenBuffer[i];
				GifQprintf("\b\b\b\b%-4d", ScreenHeight - i);
				for (j = 0, BufferP = Buffer; j < ScreenWidth; j++) {
					ColorMapEntry = &ColorMap->Colors[GifRow[j]];
					*BufferP++ = MAKE_PIXEL(ColorMapEntry->Red, ColorMapEntry->Green, ColorMapEntry->Blue);
					//                *BufferP++ = ColorMapEntry->Red;
					//                *BufferP++ = ColorMapEntry->Green;
					//                *BufferP++ = ColorMapEntry->Blue;
				}
				if (fwrite(Buffer, ScreenWidth, 1, f[0]) != 1)
					return;
				//                GIF_EXIT("Write to file(s) failed.");
				
				fwrite("\n", 1, 1, f[0]);
			}
			
			
			//        if ((Buffer = (unsigned char *) malloc(ScreenWidth * 3)) == NULL)
			//			return ;
			////            GIF_EXIT("Failed to allocate memory required, aborted.");
			//        for (i = 0; i < ScreenHeight; i++) {
			//            GifRow = ScreenBuffer[i];
			//            GifQprintf("\b\b\b\b%-4d", ScreenHeight - i);
			//            for (j = 0, BufferP = Buffer; j < ScreenWidth; j++) {
			//                ColorMapEntry = &ColorMap->Colors[GifRow[j]];
			//                *BufferP++ = ColorMapEntry->Red;
			//                *BufferP++ = ColorMapEntry->Green;
			//                *BufferP++ = ColorMapEntry->Blue;
			//            }
			//            if (fwrite(Buffer, ScreenWidth * 3, 1, f[0]) != 1)
			//				return;
			////                GIF_EXIT("Write to file(s) failed.");
			//        }
			
			free((char *) Buffer);
			fclose(f[0]);
		} else {
			unsigned char *Buffers[3];
			
			if ((Buffers[0] = (unsigned char *) malloc(ScreenWidth)) == NULL ||
				(Buffers[1] = (unsigned char *) malloc(ScreenWidth)) == NULL ||
				(Buffers[2] = (unsigned char *) malloc(ScreenWidth)) == NULL)
				return;
			//            GIF_EXIT("Failed to allocate memory required, aborted.");
			
			for (i = 0; i < ScreenHeight; i++) {
				GifRow = ScreenBuffer[i];
				GifQprintf("\b\b\b\b%-4d", ScreenHeight - i);
				for (j = 0; j < ScreenWidth; j++) {
					ColorMapEntry = &ColorMap->Colors[GifRow[j]];
					Buffers[0][j] = ColorMapEntry->Red;
					Buffers[1][j] = ColorMapEntry->Green;
					Buffers[2][j] = ColorMapEntry->Blue;
				}
				if (fwrite(Buffers[0], ScreenWidth, 1, f[0]) != 1 ||
					fwrite(Buffers[1], ScreenWidth, 1, f[1]) != 1 ||
					fwrite(Buffers[2], ScreenWidth, 1, f[2]) != 1)
					return;
				//                GIF_EXIT("Write to file(s) failed.");
			}
			
			free((char *) Buffers[0]);
			free((char *) Buffers[1]);
			free((char *) Buffers[2]);
			fclose(f[0]);
			fclose(f[1]);
			fclose(f[2]);
		}
	}
	
	static void GIF2RGB(int NumFiles, char *FileName,
						bool OneFileFlag,
						char *OutFileName)
	{
		int	i, j, Size, Row, Col, Width, Height, ExtCode, Count;
		GifRecordType RecordType;
		GifByteType *Extension;
		GifRowType *ScreenBuffer;
		GifFileType *GifFile;
		int
		InterlacedOffset[] = { 0, 4, 2, 1 }, /* The way Interlaced image should. */
		InterlacedJumps[] = { 8, 8, 4, 2 };    /* be read - offsets and jumps... */
		int ImageNum = 0;
		ColorMapObject *ColorMap;
		
		if (NumFiles == 1) {
			int Error;
			if ((GifFile = DGifOpenFileName(FileName, &Error)) == NULL) {
				//			PrintGifError(Error);
				exit(EXIT_FAILURE);
			}
		}
		else {
			int Error;
			/* Use stdin instead: */
			if ((GifFile = DGifOpenFileHandle(0, &Error)) == NULL) {
				//			PrintGifError(Error);
				exit(EXIT_FAILURE);
			}
		}
		
		/*
		 * Allocate the screen as vector of column of rows. Note this
		 * screen is device independent - it's the screen defined by the
		 * GIF file parameters.
		 */
		if ((ScreenBuffer = (GifRowType *)
			 malloc(GifFile->SHeight * sizeof(GifRowType))) == NULL)
			return ;
		//	    GIF_EXIT("Failed to allocate memory required, aborted.");
		
		Size = GifFile->SWidth * sizeof(GifPixelType);/* Size in bytes one row.*/
		if ((ScreenBuffer[0] = (GifRowType) malloc(Size)) == NULL) /* First row. */
			return ;
		//		GIF_EXIT("Failed to allocate memory required, aborted.");
		
		for (i = 0; i < GifFile->SWidth; i++)  /* Set its color to BackGround. */
			ScreenBuffer[0][i] = GifFile->SBackGroundColor;
		for (i = 1; i < GifFile->SHeight; i++) {
			/* Allocate the other rows, and set their color to background too: */
			if ((ScreenBuffer[i] = (GifRowType) malloc(Size)) == NULL)
				return;
			//			GIF_EXIT("Failed to allocate memory required, aborted.");
			
			memcpy(ScreenBuffer[i], ScreenBuffer[0], Size);
		}
		
		/* Scan the content of the GIF file and load the image(s) in: */
		do {
			if (DGifGetRecordType(GifFile, &RecordType) == GIF_ERROR) {
				//			PrintGifError(GifFile->Error);
				exit(EXIT_FAILURE);
			}
			switch (RecordType) {
				case IMAGE_DESC_RECORD_TYPE:
					if (DGifGetImageDesc(GifFile) == GIF_ERROR) {
						//					PrintGifError(GifFile->Error);
						exit(EXIT_FAILURE);
					}
					Row = GifFile->Image.Top; /* Image Position relative to Screen. */
					Col = GifFile->Image.Left;
					Width = GifFile->Image.Width;
					Height = GifFile->Image.Height;
					GifQprintf("\nImage %d at (%d, %d) [%dx%d]:     ",
							   ++ImageNum, Col, Row, Width, Height);
					if (GifFile->Image.Left + GifFile->Image.Width > GifFile->SWidth ||
						GifFile->Image.Top + GifFile->Image.Height > GifFile->SHeight) {
						fprintf(stderr, "Image %d is not confined to screen dimension, aborted.\n",ImageNum);
						exit(EXIT_FAILURE);
					}
					if (GifFile->Image.Interlace) {
						/* Need to perform 4 passes on the images: */
						for (Count = i = 0; i < 4; i++)
							for (j = Row + InterlacedOffset[i]; j < Row + Height;
								 j += InterlacedJumps[i]) {
								GifQprintf("\b\b\b\b%-4d", Count++);
								if (DGifGetLine(GifFile, &ScreenBuffer[j][Col],
												Width) == GIF_ERROR) {
									PrintGifError(GifFile->Error);
									exit(EXIT_FAILURE);
								}
							}
					}
					else {
						for (i = 0; i < Height; i++) {
							GifQprintf("\b\b\b\b%-4d", i);
							if (DGifGetLine(GifFile, &ScreenBuffer[Row++][Col],
											Width) == GIF_ERROR) {
								PrintGifError(GifFile->Error);
								exit(EXIT_FAILURE);
							}
						}
					}
					break;
				case EXTENSION_RECORD_TYPE:
					/* Skip any extension blocks in file: */
					if (DGifGetExtension(GifFile, &ExtCode, &Extension) == GIF_ERROR) {
						PrintGifError(GifFile->Error);
						exit(EXIT_FAILURE);
					}
					while (Extension != NULL) {
						if (DGifGetExtensionNext(GifFile, &Extension) == GIF_ERROR) {
							PrintGifError(GifFile->Error);
							exit(EXIT_FAILURE);
						}
					}
					break;
				case TERMINATE_RECORD_TYPE:
					break;
				default:		    /* Should be trapped by DGifGetRecordType. */
					break;
			}
		} while (RecordType != TERMINATE_RECORD_TYPE);
		
		/* Lets dump it - set the global variables required and do it: */
		ColorMap = (GifFile->Image.ColorMap
					? GifFile->Image.ColorMap
					: GifFile->SColorMap);
		if (ColorMap == NULL) {
			fprintf(stderr, "Gif Image does not have a colormap\n");
			exit(EXIT_FAILURE);
		}
		
		DumpScreen2RGB(OutFileName, OneFileFlag,
					   ColorMap,
					   ScreenBuffer,
					   GifFile->SWidth, GifFile->SHeight);
		
		(void)free(ScreenBuffer);
		
		if (DGifCloseFile(GifFile) == GIF_ERROR) {
			PrintGifError(GifFile->Error);
			exit(EXIT_FAILURE);
		}
		
	}

	
	gifToRgb

}