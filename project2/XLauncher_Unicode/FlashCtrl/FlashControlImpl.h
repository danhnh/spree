
#include "FlashControl.h" // CFlashControlInterface 가 들어있는 헤더파일

#include "FSControl.h"

// CFlashControlInterfaceImpl 은 CFlashControlInterface 클래스의 virtual 함수들을
// 실제로 구현한 클래스이며, 내부적으로 NativePlayerWnd 를 사용합니다.
class CFlashControlInterfaceImpl: public CFlashControlInterface
{
public:
	CFlashControlInterfaceImpl();
	virtual	~CFlashControlInterfaceImpl();

	NativePlayerWnd m_ctrPlayer;

public:
	virtual	void initialize( HWND hwnd );
	virtual	HDC  GetNWindowDC() ;				// This is a call for the Flash code to use
	virtual	void SetDC( HDC dc ) ;  // Only for use by the Window Message Handler
	virtual	void PlayTimerFire() ;
	virtual BOOL UpdateCursor() ;	
	virtual	void CursorTimerFire() ;

	virtual void Repaint(RECT sr) ;
	virtual void HandleKeyDown(int key, int modifiers) ;
	virtual void MouseMove(int x, int y, BOOL mouseIsDown) ;
	virtual void ControlOpen(char* filename ) ;
	virtual void FreeBuffer() ;
	virtual void MouseUp(int x, int y ) ;
	virtual void MouseDown(int x, int y );

	virtual void ControlZoomIn();
	virtual void ControlZoomOut();
	virtual void ControlViewAll();
	virtual void ControlHighQuality();
	virtual void SetLowQuality();
	virtual void ControlPlay();
	virtual void ControlLoop();
	virtual void ControlRewind();
	virtual void ControlForward();
	virtual void ControlBack();
	virtual void ControlClose();
};

CFlashControlInterface* GetFlashControl();