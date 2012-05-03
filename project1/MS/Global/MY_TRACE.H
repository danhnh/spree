/*=========================================================
Trace Util
class CSystemTrace, CConvertHTMLFormationString

����
Trace �α׸� ������ �����ġ�� ����Ѵ�. 
Trace log ������ �̸� ������ �����ؼ� Trace ID �� �����ϸ�
���ÿ� ���� Trace�� �����ϴ�. �ǽð����� ��� ���θ� �����ؼ�
���ʿ��Ҷ� ������ �ʿ��Ҷ��� Trace�ϵ��� ������ �� �ִ�.
File �� ����Ұ�� HTML �������� ����Ͽ� Trace log ���뿡 ����
Color �� �����ϰų� BLINK �ɼ��� �����ϸ� �α� �˻��� �����ϴ�.

2004.06.03		by. ozzywow 
==========================================================*/

#pragma	  once


namespace _ns_trace_ 
{
	enum _TraceFormat								// Log string format code
	{
		_FORMAT_TEXT = 0,
		_FORMAT_HTML
	};

	enum _FontColor									// Log string color code
	{
		_FONT_COLOR_RED = 0xff0000,
		_FONT_COLOR_GREEN = 0x00ff00,
		_FONT_COLOR_BLUE = 0x0000ff,
		_FONT_COLOR_BLACK = 0x000000,
	};
} ;

// BLINK option ( for HTML format log )
#define _BLINK_		true		
#define _NBLINK_	false

// Trace log buf size
#define _TRACE_HEAD_BUF_SIZE	128					// log header buf size
#define _TRACE_BADY_BUF_SIZE	1024				// log bady buf size

// Trace data 
struct  _TraceData {
	bool	bActive ;								// �ǽð����� ������� ����( true : on, false : off )
	char	cHeadTitle[_TRACE_HEAD_BUF_SIZE] ;		// head ( ����� ������ �����ؼ� ����Ҷ� ������ �����ϰ� )
	char	cBuf[_TRACE_BADY_BUF_SIZE] ;			// bady ( Ʈ���̽� �α� ���� )
	CRITICAL_SECTION	cs ;						// TRACE ID ���� ����ȭ ��Ų��.
} ;	






// Trace base class
class CSystemTrace
{

private:

	int			m_iTraceSize ;						// ��ϵ� Trace ����
	char		m_cTraceFormat ;					// Trace string format
	_TraceData * m_pTraceData ;			



public:

	CSystemTrace( const int iSize, const char cFormat = _ns_trace_::_FORMAT_TEXT ) 
		: m_iTraceSize(iSize)
		, m_cTraceFormat(cFormat)
	{		
		if( iSize <= 0 )
		{
			throw ;
		}

		m_pTraceData = new _TraceData[m_iTraceSize] ;
		_TraceData * pTrace = m_pTraceData ;
		ZeroMemory( m_pTraceData, sizeof(_TraceData) * m_iTraceSize ) ;

		// TraceData Object �ʱ�ȭ
		for( int i = 0  ; i < m_iTraceSize ; i++ )
		{			
#if (_WIN32_WINNT > 0x0403)
			if( FALSE == InitializeCriticalSectionAndSpinCount( &pTrace->cs, (0x80000000)|2000 ) )
			{
				throw ;
			}
#else
			InitializeCriticalSection( &pTrace->cs ) ;
#endif
			++pTrace ;
		}		
	}

	virtual ~CSystemTrace()
	{
		_TraceData * pTrace = m_pTraceData ;
		for( int i = 0  ; i < m_iTraceSize ; i++ )
		{
			DeleteCriticalSection( &pTrace->cs ) ;
			++pTrace ;
		}

		delete [] m_pTraceData ;
	}





	// Set header title of trace
	inline void SetHeadTitle(  const int iTraceClassNum, const char * cHeadTitle )
	{
		if( iTraceClassNum > 0 && iTraceClassNum < m_iTraceSize )
		{
			memcpy( (&m_pTraceData[iTraceClassNum])->cHeadTitle, cHeadTitle, strlen(cHeadTitle) ) ;
		}		
	}	

	// Show header title list
	_TraceData * ShowHeadTitleList()
	{
		return m_pTraceData ;
	}

	// Print trace log string
	inline void OutputString( const int iTraceClassNum /*TRACE ID*/ , const char * cLogString,... ) 
	{
		if( iTraceClassNum > 0 && iTraceClassNum < m_iTraceSize )
		{
			_TraceData * pTrace = m_pTraceData + iTraceClassNum ;
			if( pTrace->bActive == true )
			{			
				EnterCriticalSection( &pTrace->cs ) ;
				va_list arglist ;
				va_start(arglist, cLogString) ;
				vsprintf( pTrace->cBuf, cLogString, arglist) ;
				va_end(arglist) ;


				// TODO : Output string function
				OutputDevice( pTrace->cHeadTitle, pTrace->cBuf ) ;			

				LeaveCriticalSection( &pTrace->cs ) ;
			}		
		}		
	}

	// Set trace mode ( �ǽð����� Trace ID ���� active ���¸� �����ؼ� �ش� trace�� ��� ���θ� �����Ѵ� )
	void ShowTraceString( const int iTraceClassNum/*TRACE ID*/ , const bool bMode /*true:show, false:hide*/)
	{
		if( iTraceClassNum > 0 && iTraceClassNum < m_iTraceSize )
		{
			_TraceData * pTrace = m_pTraceData + iTraceClassNum ;
			pTrace->bActive = bMode ;
		}
	}

	// TODO : ��ӹ޴� Class ���� �� �Լ��� ������ �ؼ� ��´��(�ܼ�â, ���� ��..)�� �������.
	virtual void OutputDevice( const char * title, const char * str ) = 0 ;
	
};







// Trace log �� ���Ϸ� ����Ҷ� HTML �������� ����ϵ��� ������ ���ش�.
class CConvertHTMLFormationString
{
public:
	static char * strBuf ;
	static char * ConvertHTMLFormation( char * str, int color = 0xffffff, bool blink = false )
	{
		if( _BLINK_ == blink )
		{
			sprintf( strBuf, "<BR><U><I><FONT COLOR = %x>%s</FONT></I></U>", color, str ) ;
		}
		else
		{
			sprintf( strBuf, "<BR><FONT COLOR = %x>%s</FONT>", color, str ) ;
		}		
		return strBuf ;
	}
};


// HTML Format ���� ��ȯ
#ifndef _DEFINE_HTML_MACRO_FUNC
#define _DEFINE_HTML_MACRO_FUNC

//#pragma warning (disable:4003)
#ifdef _USE_HTML_FORMAION_
// HTML Format ���� ��ȯ
#define HTML(str, color, blink )			( CConvertHTMLFormationString::ConvertHTMLFormation(str, color, blink ) )
#else
#define HTML(str, color, blink )			( str )
#endif

#endif