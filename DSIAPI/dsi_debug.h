#ifndef DSI_DEBUG_H
#define DSI_DEBUG_H

int Message( const char * msg, int debugLevel )
{
    return fprintf( stderr, "DSI Message (level %d): %s\n", debugLevel, msg );
}

int CheckError( void )
{
    if( DSI_Error() ) return printf( "%s\n", DSI_ClearError() );
    else return 0;
}
#define CHECK     if( CheckError() != 0 ) return -1;



#endif // DSI_DEBUG_H
