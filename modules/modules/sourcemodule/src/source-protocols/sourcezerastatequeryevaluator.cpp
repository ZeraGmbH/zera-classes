#include "sourcezerastatequeryevaluator.h"

bool SourceZeraStateQueryEvaluator::evalQueryResponse(QByteArray queryContentBytes)
{
    bool passRoughEvaluationOnlyDecodingDetailsIsNotOurJob = true;
    if(!queryContentBytes.isEmpty()) {
        for(int i=0; i<queryContentBytes.size(); ++i) {
            if(queryContentBytes[i] != '0') {
                passRoughEvaluationOnlyDecodingDetailsIsNotOurJob = false;
                break;
            }
        }
    }
    return passRoughEvaluationOnlyDecodingDetailsIsNotOurJob;
}
