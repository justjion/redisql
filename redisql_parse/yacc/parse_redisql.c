#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse_redisql.h"

//golble list
SqlNode g_stSql;

void init()
{
	g_stSql.nType = REDISQL_EMPTY;
	g_stSql.pcDatabaseName = NULL;
	g_stSql.pcTableName = NULL;
	g_stSql.pcIndexName = NULL;
	g_stSql.nFieldTypeNum = 0;
	g_stSql.pstFieldType = NULL;
	g_stSql.nFieldValueNum = 0;
	g_stSql.pstFieldValue = NULL;
	g_stSql.nFieldAliasNum = 0;
	g_stSql.pstFieldAlias = NULL;
	g_stSql.nTableAliasNum = 0;
	g_stSql.pstFrom = NULL;
	g_stSql.pcWhere = NULL;
	g_stSql.nTop = 0;
}

void setType(int nType)
{
	g_stSql.nType = nType;
}

int setDatabaseName(const char * pcDatabaseName)
{
	int nLen = 0;

	nLen = strlen(pcDatabaseName);

	g_stSql.pcDatabaseName = (char *)malloc(nLen);

	if (NULL == g_stSql.pcDatabaseName)
	{
		return MALLOC_ERR;
	}

	strcpy(g_stSql.pcDatabaseName, pcDatabaseName);

	return 0;
}

int setTableName(const char * pcTableName)
{
	int nLen = 0;

	nLen = strlen(pcTableName);

	g_stSql.pcTableName = (char *)malloc(nLen);

	if (NULL == g_stSql.pcTableName)
	{
		return MALLOC_ERR;
	}

	strcpy(g_stSql.pcTableName, pcTableName);

	return 0;
}

int setIndexName(const char * pcIndexName)
{
	int nLen = 0;

	nLen = strlen(pcIndexName);

	g_stSql.pcIndexName = (char *)malloc(nLen);

	if (NULL == g_stSql.pcIndexName)
	{
		return MALLOC_ERR;
	}

	strcpy(g_stSql.pcIndexName, pcIndexName);

	return 0;
}

int addFieldType(const char * pcField, const char * pcType)
{
	int nLenField = 0;
	int nLenType = 0;
	FieldType * pstTmpFieldType = NULL;
	FieldType * pstTmp = NULL;

	pstTmpFieldType = mallocFieldType();
	if (NULL == pstTmpFieldType)
	{
		return MALLOC_ERR;
	}
	nLenField = strlen(pcField);
	nLenType = strlen(pcType);
	pstTmpFieldType->pcField = (char *)malloc(nLenField);
	if (NULL == pstTmpFieldType->pcField)
	{
		destoryFieldType(pstTmpFieldType);
		return MALLOC_ERR;
	}
	strcpy(pstTmpFieldType->pcField, pcField);

	pstTmpFieldType->pcType = (char *)malloc(nLenType);
	if (NULL == pstTmpFieldType->pcType)
	{
		destoryFieldType(pstTmpFieldType);
		return MALLOC_ERR;
	}
	strcpy(pstTmpFieldType->pcType, pcType);
	if (NULL == g_stSql.pstFieldType)
	{
		g_stSql.pstFieldType = pstTmpFieldType;
	}else{
		pstTmp = g_stSql.pstFieldType;
		while(pstTmp->pstNextField)
		{
			pstTmp = pstTmp->pstNextField;
		}
		pstTmp->pstNextField = pstTmpFieldType;
	}
	g_stSql.nFieldTypeNum++;
	return 0;
}

int addFieldValue(int nFieldType, union Value uValue)
{
	FieldValue * pstTmpFieldValue = NULL;
	FieldValue * pstTmp = NULL;

	pstTmpFieldValue = mallocFieldValue();
	if (NULL == pstTmpFieldValue)
	{
		return MALLOC_ERR;
	}

	pstTmpFieldValue->nFieldType = nFieldType;
	
	if (nFieldType == REDISQL_INT)
	{
		pstTmpFieldValue->uValue.nValue = uValue.nValue;
	}
	else if (nFieldType == REDISQL_STRING)
	{
		int nLenValue = 0;
		nLenValue = strlen(uValue.pcValue);
		pstTmpFieldValue->uValue.pcValue = (char *)malloc(sizeof(nLenValue));
		if (NULL == pstTmpFieldValue)
		{
			destoryFieldValue(pstTmpFieldValue);
			return MALLOC_ERR;
		}
		strcpy(pstTmpFieldValue->uValue.pcValue, uValue.pcValue);
	}
	else if (nFieldType == REDISQL_FLOAT)
	{
		pstTmpFieldValue->uValue.fValue = uValue.fValue;
	}
	else
	{
		return TYPE_ERR;
	}

	if (NULL == g_stSql.pstFieldValue)
	{
		g_stSql.pstFieldValue = pstTmpFieldValue;
	}else{
		pstTmp = g_stSql.pstFieldValue;
		while(pstTmp->pstNextField)
		{
			pstTmp = pstTmp->pstNextField;
		}
		pstTmp->pstNextField = pstTmpFieldValue;
	}

	g_stSql.nFieldValueNum++;
	return 0;
}

int addFieldAlias(const char * pcTableAlias, const char * pcField, const char * pcAlias)
{
	FieldAlias * pstTmpFieldAlias = NULL;
	FieldAlias * pstTmp = NULL;
	int nLenTable = 0;
	int nLenField = 0;
	int nLenAlias = 0;

	pstTmpFieldAlias = mallocFieldAlias();
	if (NULL == pstTmpFieldAlias)
	{
		return MALLOC_ERR;
	}

	nLenTable = strlen(pcTableAlias);
	nLenField = strlen(pcField);
	nLenAlias = strlen(pcAlias);

	pstTmpFieldAlias->pcTableAlias = (char *)malloc(nLenTable);
	if (NULL == pstTmpFieldAlias->pcTableAlias)
	{
		destoryFieldAlias(pstTmpFieldAlias);
		return MALLOC_ERR;
	}
	strcpy(pstTmpFieldAlias->pcTableAlias, pcTableAlias);

	pstTmpFieldAlias->pcField = (char *)malloc(nLenField);
	if (NULL == pstTmpFieldAlias->pcField)
	{
		destoryFieldAlias(pstTmpFieldAlias);
		return MALLOC_ERR;
	}
	strcpy(pstTmpFieldAlias->pcField, pcField);

	pstTmpFieldAlias->pcAlias = (char *)malloc(nLenAlias);
	if (NULL == pstTmpFieldAlias->pcAlias)
	{
		destoryFieldAlias(pstTmpFieldAlias);
		return MALLOC_ERR;
	}
	strcpy(pstTmpFieldAlias->pcAlias, pcAlias);

	if (NULL == g_stSql.pstFieldAlias)
	{
		g_stSql.pstFieldAlias = pstTmpFieldAlias;
	}else{
		pstTmp = g_stSql.pstFieldAlias;
		while(pstTmp->pstNextField)
		{
			pstTmp = pstTmp->pstNextField;
		}
		pstTmp->pstNextField = pstTmpFieldAlias;
	}

	g_stSql.nFieldAliasNum++;
	return 0;
}

int addTableAlias(const char * pcTable, const char * pcAlias)
{
	TableAlias * pstTmpTableAlias = NULL;
	TableAlias * pstTmp = NULL;
	int nLenTable = 0;
	int nLenAlias = 0;

	pstTmpTableAlias = mallocTableAlias();

	if (NULL == pstTmpTableAlias)
	{
		return MALLOC_ERR;
	}

	nLenTable = strlen(pcTable);
	nLenAlias = strlen(pcAlias);

	pstTmpTableAlias->pcTable = (char *)malloc(nLenTable);
	if (NULL == pstTmpTableAlias->pcTable)
	{
		destoryTableAlias(pstTmpTableAlias);
		return MALLOC_ERR;
	}
	strcpy(pstTmpTableAlias->pcTable, pcTable);

	pstTmpTableAlias->pcAlias = (char *)malloc(nLenAlias);
	if (NULL == pstTmpTableAlias->pcAlias)
	{
		destoryTableAlias(pstTmpTableAlias);
		return MALLOC_ERR;
	}
	strcpy(pstTmpTableAlias->pcAlias, pcAlias);

	if (NULL == g_stSql.pstFrom)
	{
		g_stSql.pstFrom = pstTmpTableAlias;
	}else{
		pstTmp = g_stSql.pstFrom;
		while(pstTmp->pstNextTable)
		{
			pstTmp = pstTmp->pstNextTable;
		}
		pstTmp->pstNextTable = pstTmpTableAlias;
	}

	g_stSql.nTableAliasNum++;
	return 0;
}

int setWhere(const char * pcWhere)
{
	int nLen = 0;

	nLen = strlen(pcWhere);

	g_stSql.pcWhere = (char *)malloc(nLen);

	if (NULL == g_stSql.pcWhere)
	{
		return MALLOC_ERR;
	}

	strcpy(g_stSql.pcWhere, pcWhere);

	return 0;
}

void setTop(int nTop)
{
	g_stSql.nTop = nTop;
}

void setLimit(int nStart, int nEnd)
{
	g_stSql.stLimit.nStart = nStart;
	g_stSql.stLimit.nEnd = nEnd;
}

FieldType * mallocFieldType()
{
	FieldType * pstRes = NULL;
	pstRes = (FieldType *)malloc(sizeof(FieldType));
	if (NULL == pstRes)
	{
		return NULL;
	}
	memset(pstRes, 0, sizeof(FieldType));
	return pstRes;
}

FieldValue * mallocFieldValue()
{
	FieldValue * pstRes = NULL;
	pstRes = (FieldValue *)malloc(sizeof(FieldValue));
	if (NULL == pstRes)
	{
		return NULL;
	}
	memset(pstRes, 0, sizeof(FieldValue));
	return pstRes;
}

FieldAlias * mallocFieldAlias()
{
	FieldAlias * pstRes = NULL;
	pstRes = (FieldAlias *)malloc(sizeof(FieldAlias));
	if (NULL == pstRes)
	{
		return NULL;
	}
	memset(pstRes, 0, sizeof(FieldAlias));
	return pstRes;
}

TableAlias * mallocTableAlias()
{
	TableAlias * pstRes = NULL;
	pstRes = (TableAlias *)malloc(sizeof(TableAlias));
	if (NULL == pstRes)
	{
		return NULL;
	}
	memset(pstRes, 0, sizeof(TableAlias));
	return pstRes;
}

void destoryFieldType(FieldType *pst)
{
	if (NULL == pst)
	{
		return;
	}

	if (NULL != pst->pcField)
	{
		free(pst->pcField);
		pst->pcField = NULL;
	}

	if (NULL != pst->pcType)
	{
		free(pst->pcType);
		pst->pcType = NULL;
	}

	free(pst);
	pst = NULL;
}

void destoryFieldValue(FieldValue *pst)
{
	if (NULL == pst)
	{
		return;
	}

	if (pst->nFieldType == REDISQL_STRING)
	{
		if (NULL != pst->uValue.pcValue)
		{
			free(pst->uValue.pcValue);
			pst->uValue.pcValue = NULL;
		}
	}

	free(pst);
	pst = NULL;
}

void destoryFieldAlias(FieldAlias *pst)
{
	if (NULL == pst)
	{
		return;
	}

	if (NULL != pst->pcTableAlias)
	{
		free(pst->pcTableAlias);
		pst->pcTableAlias = NULL;
	}

	if (NULL != pst->pcField)
	{
		free(pst->pcField);
		pst->pcField = NULL;
	}

	if (NULL != pst->pcAlias)
	{
		free(pst->pcAlias);
		pst->pcAlias = NULL;
	}

	free(pst);
	pst = NULL;
}

void destoryTableAlias(TableAlias *pst)
{
	if (NULL == pst)
	{
		return;
	}

	if (NULL != pst->pcTable)
	{
		free(pst->pcTable);
		pst->pcTable = NULL;
	}

	if (NULL != pst->pcAlias)
	{
		free(pst->pcAlias);
		pst->pcAlias = NULL;
	}

	free(pst);
	pst = NULL;
}

void destorySqlNode()
{
	FieldType * pstTmpFieldType = NULL;
	FieldValue * pstTmpFieldValue = NULL;
	FieldAlias * pstTmpFieldAlias = NULL;
	TableAlias * pstTmpTableAlias = NULL;
	FieldType * pstFreeFieldType = NULL;
	FieldValue * pstFreeFieldValue = NULL;
	FieldAlias * pstFreeFieldAlias = NULL;
	TableAlias * pstFreeTableAlias = NULL;

	if (NULL != g_stSql.pcDatabaseName)
	{
		free(g_stSql.pcDatabaseName);
		g_stSql.pcDatabaseName = NULL;
	}

	if (NULL != g_stSql.pcTableName)
	{
		free(g_stSql.pcTableName);
		g_stSql.pcTableName = NULL;
	}

	if (NULL != g_stSql.pcIndexName)
	{
		free(g_stSql.pcIndexName);
		g_stSql.pcIndexName = NULL;
	}

	pstTmpFieldType = g_stSql.pstFieldType;
	while(pstTmpFieldType)
	{
		pstFreeFieldType = pstTmpFieldType;
		pstTmpFieldType = pstTmpFieldType->pstNextField;
		destoryFieldType(pstFreeFieldType);
	}

	pstTmpFieldValue = g_stSql.pstFieldValue;
	while(pstTmpFieldValue)
	{
		pstFreeFieldValue = pstTmpFieldValue;
		pstTmpFieldValue = pstTmpFieldValue->pstNextField;
		destoryFieldValue(pstFreeFieldValue);
	}

	pstTmpFieldAlias = g_stSql.pstFieldAlias;
	while(pstTmpFieldAlias)
	{
		pstFreeFieldAlias = pstTmpFieldAlias;
		pstTmpFieldAlias = pstTmpFieldAlias->pstNextField;
		destoryFieldAlias(pstFreeFieldAlias);
	}

	pstTmpTableAlias = g_stSql.pstFrom;
	while(pstTmpTableAlias)
	{
		pstFreeTableAlias = pstTmpTableAlias;
		pstTmpTableAlias = pstTmpTableAlias->pstNextTable;
		destoryTableAlias(pstFreeTableAlias);
	}

	if (NULL != g_stSql.pcWhere)
	{
		free(g_stSql.pcWhere);
		g_stSql.pcWhere = NULL;
	}
}

void showSql()
{
	printf("num: %d %d %d %d\n", g_stSql.nFieldTypeNum, g_stSql.nFieldValueNum, g_stSql.nFieldAliasNum, g_stSql.nTableAliasNum);
	switch(g_stSql.nType)
	{
		case REDISQL_USE:
		{
			printf("USE %s;\n", g_stSql.pcDatabaseName);
			break;
		}
		case REDISQL_SHOW_DATABASES:
		{
			printf("SHOW DATABASES;\n");
			break;
		}
		case REDISQL_SHOW_TABLES:
		{
			printf("SHOW TABLES;\n");
			break;
		}
		case REDISQL_SHOW_INDEX:
		{
			printf("SHOW INDEX FROM %s;\n", g_stSql.pcTableName);
			break;
		}
		case REDISQL_DESC:
		{
			printf("DESC %s;\n", g_stSql.pcTableName);
			break;
		}
		case REDISQL_CREATE_DATABASE:
		{
			printf("CREATE DATABASE %s;\n", g_stSql.pcDatabaseName);
			break;
		}
		case REDISQL_CREATE_TABLE:
		{
			FieldType *pstTmp = NULL;
			printf("CREATE TABLE %s(", g_stSql.pcTableName);
			pstTmp = g_stSql.pstFieldType;
			while(pstTmp)
			{
				printf("%s %s", pstTmp->pcField, pstTmp->pcType);
				pstTmp = pstTmp->pstNextField;
				if (NULL != pstTmp)
				{
					printf(", ");
				}
			}
			printf(");\n");
			break;
		} 
		case REDISQL_CREATE_INDEX:
		{
			FieldType *pstTmp = NULL;
			printf("CREATE INDEX %s ON %s(", g_stSql.pcIndexName, g_stSql.pcTableName);
			pstTmp = g_stSql.pstFieldType;
			while(pstTmp)
			{
				printf("%s", pstTmp->pcField);
				pstTmp = pstTmp->pstNextField;
				if (NULL != pstTmp)
				{
					printf(", ");
				}
			}
			printf(");\n");
			break;
		}
		case REDISQL_INSERT:
		{
			FieldType *pstType = NULL;
			FieldValue *pstValue = NULL;
			printf("INSERT INTO %s(", g_stSql.pcTableName);
			pstType = g_stSql.pstFieldType;
			while(pstType)
			{
				printf("%s", pstType->pcField);
				pstType = pstType->pstNextField;
				if (NULL != pstType)
				{
					printf(", ");
				}
			}
			printf(") VALUES(");
			pstValue = g_stSql.pstFieldValue;
			while(pstValue)
			{
				if (pstValue->nFieldType == REDISQL_INT)
				{
					printf("%d", pstValue->uValue.nValue);
				}
				else if(pstValue->nFieldType == REDISQL_FLOAT)
				{
					printf("%f", pstValue->uValue.fValue);
				}
				else
				{
					printf("%s", pstValue->uValue.pcValue);
				}
				pstValue = pstValue->pstNextField;
				if (NULL != pstValue)
				{
					printf(", ");
				}
			}
			printf(");\n");
			break;
		}
		case REDISQL_SELECT:
		{
			FieldAlias * pstField = NULL;
			TableAlias * pstTable = NULL;
			printf("SELECT ");
			pstField = g_stSql.pstFieldAlias;
			while(pstField)
			{
				if (pstField->pcTableAlias != NULL)
				{
					if (strcmp(pstField->pcTableAlias, "") != 0)
					{
						printf("%s.", pstField->pcTableAlias);
					}
				}
				printf("%s", pstField->pcField);
				if (pstField->pcAlias != NULL)
				{
					printf(" %s", pstField->pcAlias);
				}
				pstField = pstField->pstNextField;
				if (NULL != pstField)
				{
					printf(", ");
				}
			}
			printf(" FROM ");
			pstTable = g_stSql.pstFrom;
			while(pstTable)
			{
				printf("%s", pstTable->pcTable);
				if (NULL != pstTable->pcTable)
				{
					if (strcmp(pstTable->pcAlias, "") != 0)
					{
						printf(" %s", pstTable->pcAlias);
					}
				}
				pstTable = pstTable->pstNextTable;
				if (NULL != pstTable)
				{
					printf(", ");
				}
			}
			if (NULL != g_stSql.pcWhere)
			{
				printf(" WHERE %s", g_stSql.pcWhere);
			} 

			if (g_stSql.nTop > 0) 
			{
				printf(" Top %d", g_stSql.nTop);
			}

			if (g_stSql.stLimit.nStart != 0 || g_stSql.stLimit.nEnd != 0)
			{
				printf(" LIMIT %d, %d", g_stSql.stLimit.nStart, g_stSql.stLimit.nEnd);
			}
			printf(";\n");
			break;
		}
		case REDISQL_UPDATE:
		{
			break;
		}
		case REDISQL_DELETE:
		{
			break;
		}
		case REDISQL_DROP_DATABASE:
		{
			break;
		}
		case REDISQL_DROP_TABLE:
		{
			break;
		}
		case REDISQL_EXIT:
		{
			printf("EXIT\n");
			break;
		}
		case REDISQL_HELP:
		{
			printf("HELP\n");
			break;
		}
		case REDISQL_EMPTY:
		{
			printf("\n");
			break;
		}
		default:
		{
			printf("err type.\n");
		}
	}
}

int getType()
{
	return g_stSql.nType;
}

char * getDatabaseName()
{
	return g_stSql.pcDatabaseName;
}

char * getTableName()
{
	return g_stSql.pcTableName;
}

char * getIndexName()
{
	return g_stSql.pcIndexName;
}

int getFieldTypeNum()
{
	return g_stSql.nFieldTypeNum;
}

int getFieldValueNum()
{
	return g_stSql.nFieldValueNum;
}

int getFieldAliasNum()
{
	return g_stSql.nFieldAliasNum;
}

int getTableAliasNum()
{
	return g_stSql.nTableAliasNum;
}

char * getFieldType(int sn)	//field and type slpit with " "
{
	FieldType *pstType = NULL;
	int i = 0;
	int nLen = 0;
	char * pcRes = NULL;
	pstType = g_stSql.pstFieldType;
	for (i = 0; i < sn; i++)
	{
		pstType = pstType->pstNextField;
	}
	nLen = strlen(pstType->pcField) + strlen(pstType->pcField) + 2;
	pcRes = (char *)malloc(nLen);
	memset(pcRes, '\0', nLen);
	sprintf(pcRes, "%s %s", pstType->pcField, pstType->pcType);
	return pcRes;	
}

char * getFieldValue(int sn)
{
	FieldType *pstType = NULL;
	FieldValue *pstValue = NULL;
	int i = 0;
	int nLen = 0;
	char * pcRes = NULL;
	pstType = g_stSql.pstFieldType;
	pstValue = g_stSql.pstFieldValue;
	for (i = 0; i < sn; i++)
	{
		pstType = pstType->pstNextField;
		pstValue = pstValue->pstNextField;
	}
	switch (pstValue->nFieldType) 
	{
		case REDISQL_INT:
		{
			char acType[64] = {'\0'};
			char acValue[64] = {'\0'};
			sprintf(acType, "%d", pstValue->nFieldType);
			sprintf(acValue, "%d", pstValue->uValue.nValue);
			nLen =  strlen(pstType->pcField) + strlen(acType) + strlen(acValue) + 3;
			pcRes = (char *)malloc(nLen);
			memset(pcRes, '\0', nLen);
			sprintf(pcRes, "%d %s %d", pstValue->nFieldType, pstType->pcField, pstValue->uValue.nValue);
			break;
		}
		case REDISQL_FLOAT:
		{
			char acType[64] = {'\0'};
			char acValue[64] = {'\0'};
			sprintf(acType, "%d", pstValue->nFieldType);
			sprintf(acValue, "%f", pstValue->uValue.fValue);
			nLen =  strlen(pstType->pcField) + strlen(acType) + strlen(acValue) + 3;
			pcRes = (char *)malloc(nLen);
			memset(pcRes, '\0', nLen);
			sprintf(pcRes, "%d %s %f", pstValue->nFieldType, pstType->pcField, pstValue->uValue.fValue);
			break;
		}
		case REDISQL_STRING:
		{
			char acType[64] = {'\0'};
			sprintf(acType, "%d", pstValue->nFieldType);
			nLen =  strlen(pstType->pcField) + strlen(acType) + strlen(pstValue->uValue.pcValue) + 3;
			pcRes = (char *)malloc(nLen);
			memset(pcRes, '\0', nLen);
			sprintf(pcRes, "%d %s %s", pstValue->nFieldType, pstType->pcField, pstValue->uValue.pcValue);
			break;
		}
		default:
		{
			break;
		}
	}
	return pcRes;
}

char * getFieldAlias(int sn)
{
	FieldAlias *pstField = NULL;
	int i = 0;
	int nLen = 0;
	char * pcRes = NULL;
	
	pstField = g_stSql.pstFieldAlias;
	for (i = 0; i < sn; i++)
	{
		pstField = pstField->pstNextField;
	}
	
	nLen = strlen(pstField->pcTableAlias) + strlen(pstField->pcField) + strlen(pstField->pcAlias) + 3;
	pcRes = (char *)malloc(nLen);
	sprintf(pcRes, "%s %s %s", pstField->pcTableAlias, pstField->pcField, pstField->pcAlias);
	return pcRes;
}

char * getTableAlias(int sn)
{
	TableAlias *pstTable = NULL;
	int i = 0;
	int nLen = 0;
	char * pcRes = NULL;
	
	pstTable = g_stSql.pstFrom;
	for (i = 0; i < sn; i++)
	{
		pstTable = pstTable->pstNextTable;
	}
	
	nLen = strlen(pstTable->pcTable) + strlen(pstTable->pcAlias) + 2;
	pcRes = (char *)malloc(nLen);
	sprintf(pcRes, "%s %s", pstTable->pcTable, pstTable->pcAlias);
	return pcRes;
}

char * getWhere()
{
	return g_stSql.pcWhere;
}

int getTop()
{
	return g_stSql.nTop;
}

char * getLimit()
{
	char * pcRes = (char *)malloc(1024);
	memset(pcRes, '\0', 1024);
	sprintf(pcRes, "%d %d", g_stSql.stLimit.nStart, g_stSql.stLimit.nEnd);
	return pcRes;
}