#import "Adjust.h"

@interface AdjustMarmalade_platform : NSObject<AdjustDelegate>

- (void)adjustFinishedTrackingWithResponse:(AIResponseData *)responseData;

@end