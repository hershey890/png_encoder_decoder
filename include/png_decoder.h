#ifndef PNG_DECODER_H
#define PNG_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Decodes a PNG file
 * @param filename The PNG file to decode
 * @return 0 if successful, negative value for failure
 */
int decodePNG(const char* filename);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // PNG_DECODER_H