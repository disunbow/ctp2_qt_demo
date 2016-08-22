#include <string.h>
#include <iostream>
#include <stdio.h>
#include "SecTraderSpi.h"
using namespace std;

//֧��֤ȯ����ҵ�񡢲�ѯҵ��

int RequestID = 0;

SecTraderSpi::SecTraderSpi()
{

}

SecTraderSpi::~SecTraderSpi()
{

}

void SecTraderSpi::Init(const char* pi_TDAdress,
                                       const char* pi_broker,
                                       const char* pi_User,
                                       const char* pi_pwd,
                                       const char* pi_prodInfo,
                                       const char* pi_authCode)
{
    zq_TDAddress = new char[strlen(pi_TDAdress)];
    strcpy(zq_TDAddress, pi_TDAdress);
    zq_BrokerID = new char[strlen(pi_broker)];
    strcpy(zq_BrokerID, pi_broker);
    zq_UserID = new char[strlen(pi_User)];
    strcpy(zq_UserID, pi_User);
    zq_Password = new char[strlen(pi_pwd)];
    strcpy(zq_Password, pi_pwd);
    zq_UserProductInfo = new char[strlen(pi_prodInfo)];
    strcpy(zq_UserProductInfo, pi_prodInfo);
    zq_AuthCode = new char[strlen(pi_authCode)];
    strcpy(zq_AuthCode, pi_authCode);
    g_SecNum = 0;
    g_TCNum = 0;
    bIsLogon = false;
    bIsgetTradingCode = false;
    bIsgetSec = false;

    m_pTdApi = CZQThostFtdcTraderApi::CreateFtdcTraderApi("");
    m_pTdApi->RegisterSpi(this);

    // ���Ĺ�˽����
    // TERT_RESTART:�ӱ������տ�ʼ�ش�
    // TERT_RESUME:���ϴ��յ�������
    // TERT_QUICK:ֻ���͵�¼��˽����������
    m_pTdApi->SubscribePublicTopic(ZQTHOST_TERT_RESTART);
    m_pTdApi->SubscribePrivateTopic(ZQTHOST_TERT_RESTART);

    // ���׷�������ַ,����������ȷ�������޷����ӣ�����
    m_pTdApi->RegisterFront(zq_TDAddress);

    // ʹ�ͻ��˿�ʼ���̨����������
    m_pTdApi->Init();
}

void SecTraderSpi::OnFrontConnected()
{
    ReqAuthenticate();
}

int SecTraderSpi::ReqAuthenticate()
{
    CZQThostFtdcReqAuthenticateField reqAuthenticate;

    strcpy(reqAuthenticate.BrokerID, zq_BrokerID);
    strcpy(reqAuthenticate.UserID, zq_UserID);
    strcpy(reqAuthenticate.UserProductInfo, zq_UserProductInfo);
    strcpy(reqAuthenticate.AuthCode, zq_AuthCode);

    int ret = m_pTdApi->ReqAuthenticate(&reqAuthenticate, ++RequestID);
//    if (ret != 0)
        printf("ReqAuthenticate Return Code = %d\n", ret);
    return ret;
}

void SecTraderSpi::OnRspAuthenticate(CZQThostFtdcRspAuthenticateField *pRspAuthenticateField, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (!pRspInfo)
    {
        printf("pRspInfo is null\n");
        return;
    }

    if (pRspInfo->ErrorID == 0)
    {
        ReqUserLogin();
    } else
    {
        printf("OnRspAuthenticate, ErrorID = 0x%x\n", pRspInfo->ErrorID);
    }
}

int SecTraderSpi::ReqUserLogin()
{
    CZQThostFtdcReqUserLoginField reqUserLogin;

    //���ݲ��Ի�������������
    strcpy(reqUserLogin.BrokerID, zq_BrokerID);
    strcpy(reqUserLogin.UserID, zq_UserID);
    strcpy(reqUserLogin.Password, zq_Password);
    set_null(reqUserLogin.UserProductInfo);
    set_null(reqUserLogin.InterfaceProductInfo);
    set_null(reqUserLogin.ProtocolInfo);
    set_null(reqUserLogin.MacAddress);
    set_null(reqUserLogin.OneTimePassword);
    set_null(reqUserLogin.ClientIPAddress);
    set_null(reqUserLogin.Lang);

    //�û���¼����
    return m_pTdApi->ReqUserLogin(&reqUserLogin, ++RequestID);
}

void SecTraderSpi::OnRspUserLogin(CZQThostFtdcRspUserLoginField *pRspUserLogin, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        printf("OnRspUserLogin, ErrorID=0x%04x, ErrMsg=%s\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {   
        bIsLogon = true;
    }
}
int SecTraderSpi::ReqTrading()
{
    CZQThostFtdcQryTradingCodeField reqTradingCode;
	///���͹�˾����
	strcpy(reqTradingCode.BrokerID, zq_BrokerID);
	///Ͷ���ߴ���
	strcpy(reqTradingCode.InvestorID, zq_UserID);
	///����������
	set_null(reqTradingCode.ExchangeID);
	///�ͻ�����
	set_null(reqTradingCode.ClientID);
	///���ױ�������
	set_null(reqTradingCode.ClientIDType);
    //�����ѯ�����˻�
	int ret = m_pTdApi->ReqQryTradingCode(&reqTradingCode, ++RequestID);
    //if (ret != 0)
        printf("ReqQryTradingCode Return Code = %d\n", ret);
    return ret;
}
void SecTraderSpi::OnRspQryTradingCode(CZQThostFtdcTradingCodeField *pTradingCode, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    printf("OnRspQryTradingCode\n");
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        printf("OnRspQryTradingCode, ErrorID=0x%04x, ErrMsg=%s\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else
    {   
        g_TCNum++;
        g_TradeCode[g_TCNum-1] = *pTradingCode;
        printf("OnRspQryTradingCode: BrokerID=%s;", pTradingCode->BrokerID);	//���͹�˾����
        printf("InvestorID=%s;",pTradingCode->InvestorID);	                    //Ͷ���ߴ���
        printf("ExchangeID=%s;",pTradingCode->ExchangeID);          	        //����������
        printf("ClientID=%s;",pTradingCode->ClientID);	                        //�ͻ�����
        printf("IsActive=%d;",pTradingCode->IsActive);	                        //�Ƿ��Ծ
        printf("ClientIDType=%c;",pTradingCode->ClientIDType);	                //���ױ�������
        printf("MarketID=%s;",pTradingCode->MarketID);	                        //�г�����
        printf("BranchID=%s;",pTradingCode->BranchID);                          //������Ӫҵ������            
        printf("isLast=%d\n" ,bIsLast);                    
        fflush(stdout);
    }
    if (bIsLast)
    {
        bIsgetTradingCode = true;
    }
}

//��ѯ֤ȯ
int SecTraderSpi::ReqQrySecurity()
{
    printf("ReqQrySecurity\n");
    CZQThostFtdcQrySecurityField  field;

    set_null(field.InstrumentID);
    set_null(field.ExchangeID);
    set_null(field.ExchangeInstID);
    set_null(field.ProductID);

    int rtn_cd = m_pTdApi->ReqQrySecurity(&field, ++RequestID);
    printf("RqeQrySecutiry Return: %d\n", rtn_cd);
    return rtn_cd;
}

//��ѯ֤ȯ��Ӧ
void SecTraderSpi::OnRspQrySecurity(CZQThostFtdcSecurityField *pSecurity, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
        printf("��ѯOnRspQrySecurityʧ�ܡ�,ErrorID=0x%04x, ErrMsg=%s\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    else if (pSecurity)
    {
        if (((strcmp(pSecurity->ProductID, "SHStock") == 0) &&
             (strcmp(pSecurity->SecurityClassID, "SHAShares") == 0)) ||
            ((strcmp(pSecurity->ProductID, "SZStock") == 0) &&
             ((strcmp(pSecurity->SecurityClassID, "SZGEM") == 0) ||
              (strcmp(pSecurity->SecurityClassID, "SZMainAShares") == 0) ||
              (strcmp(pSecurity->SecurityClassID, "SZSME") == 0))))
        {
            g_SecNum++;
            g_SecInfo[g_SecNum - 1] = *pSecurity;
/*
            printf("ExchangeID=[%s];", pSecurity->ExchangeID);
            printf("InstrumentID=[%s];", pSecurity->InstrumentID);
            printf("PriductID=[%s];", pSecurity->ProductID);    //��Ʒ����
            printf("SecurityClassID=[%s];\n", pSecurity->SecurityClassID); 
            fflush(stdout);
*/
        }
    }
    else
        printf("no records\n");
    if (bIsLast)
        bIsgetSec = true;
}
