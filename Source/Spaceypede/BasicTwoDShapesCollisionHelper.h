// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
//#include "UObject/NoExportTypes.h"
#include <cmath>
#include "Math/Vector2D.h"

#include "BasicTwoDShapesCollisionHelper.generated.h"

const float EPSILON = 0.0001;

UENUM()
enum FEnumTypeOfHitbox
{
	Square      UMETA(DisplayName = "Square"),
	Ellipse     UMETA(DisplayName = "Ellipse"),
};

USTRUCT()
struct FTwoPoints {
	GENERATED_BODY()

	FVector2D firstPoint;
	bool bFirstExists;
	FVector2D secondPoint;
	bool bSecondExists;
};

USTRUCT()
struct FLineSegmentPoints {
	GENERATED_BODY()

	FVector2D firstPoint;
	FVector2D secondPoint;

	FLineSegmentPoints() {}

	FLineSegmentPoints(const FVector2D first, const FVector2D second) {
		firstPoint = first;
		secondPoint = second;
	}

	bool IsLinePointInBounds(const FVector2D pointOnLine) const {
		if (
			(firstPoint.X > secondPoint.X && (pointOnLine.X < firstPoint.X && pointOnLine.X > secondPoint.X)) ||
			(firstPoint.X < secondPoint.X && (pointOnLine.X > firstPoint.X && pointOnLine.X < secondPoint.X))
			)
		{
			if (
				(firstPoint.Y > secondPoint.Y && (pointOnLine.Y < firstPoint.Y && pointOnLine.Y > secondPoint.Y)) ||
				(firstPoint.Y < secondPoint.Y && (pointOnLine.Y > firstPoint.Y && pointOnLine.Y < secondPoint.Y))
				)
			{
				return true;
			}
		}
		return false;
	}
};

USTRUCT()
struct FLineEquation { //y = K * x + B
	GENERATED_BODY()

	float equationK;
	float equationB;
	bool bKinfinite; //x = x(1) = B

	FLineEquation () {}

	FLineEquation (const FLineSegmentPoints LineSegmentPoints) {
		float dx = LineSegmentPoints.secondPoint.X - LineSegmentPoints.firstPoint.X;
		float dy = LineSegmentPoints.secondPoint.Y - LineSegmentPoints.firstPoint.Y;
		if ((dx > 0 && dx < EPSILON) || (dx <= 0 && dx > EPSILON * -1)) {
			bKinfinite = true;
			equationB = LineSegmentPoints.firstPoint.X;
		}
		else {
			bKinfinite = false;
			float K = dy / dx;
			equationK = K;
			float B = LineSegmentPoints.firstPoint.Y - LineSegmentPoints.firstPoint.X * K; //plugging in first point
			equationB = B;
		}
	}

	bool CheckPointIsOnLine(const FVector2D point) const {
		if (bKinfinite) {
			float dx = point.X - equationB;
			if ((dx > 0 && dx < EPSILON) || (dx <= 0 && dx > EPSILON * -1)) {
				return true;
			}
		}
		else {
			float expectedY = point.X * equationK + equationB;

			float dy = point.Y - expectedY;
			if ((dy > 0 && dy < EPSILON) || (dy <= 0 && dy > EPSILON * -1)) {
				return true;
			}
		}
		return false;
	}

	static FVector2D FindLineCrossPoint(const FLineEquation firstLine, const FLineEquation secondLine) { //precon: equationK(1) != equationK(2)
		float x;
		float y;
		if (firstLine.bKinfinite) {
			x = firstLine.equationB;
			y = secondLine.equationK * x + secondLine.equationB;
		}
		else if (secondLine.bKinfinite) {
			x = secondLine.equationB;
			y = firstLine.equationK * x + firstLine.equationB;
		}
		else {
			float dk = firstLine.equationK - secondLine.equationK;
			if ((dk > 0 && dk < EPSILON) || (dk <= 0 && dk > EPSILON * -1)) { //should never happen if precondition is true
				//return vector (0,0)
			}
			else {
				x = (secondLine.equationB - firstLine.equationB) / dk;
				y = firstLine.equationK * x + firstLine.equationB;
			}
		}
		return FVector2D(x, y);
	}

	static bool CheckSegmentsCross(FLineSegmentPoints firstSegment, FLineSegmentPoints secondSegment) {
		FLineEquation firstLine = FLineEquation(firstSegment);
		FLineEquation secondLine = FLineEquation(secondSegment);

		//niche case where lines are the same
		float dk = firstLine.equationK - secondLine.equationK;

		if ((dk > 0 && dk < EPSILON) || (dk <= 0 && dk > EPSILON * -1)) {

			float db = firstLine.equationB - secondLine.equationB;
			if ((db > 0 && db < EPSILON) || (db <= 0 && db > EPSILON * -1)) {

				if (firstSegment.IsLinePointInBounds(secondSegment.firstPoint) || firstSegment.IsLinePointInBounds(secondSegment.secondPoint)) {
					return true;
				}
				else {
					return true;
				}
			}
			else {
				return false;
			}
		}

		FVector2D crossPoint = FindLineCrossPoint(firstLine, secondLine);
		return firstSegment.IsLinePointInBounds(crossPoint);
	}
};

USTRUCT()
struct FTwoDColliderVertexForm {
	GENERATED_BODY()

	TEnumAsByte<FEnumTypeOfHitbox> EnumTypeOfHitbox;
	FVector2D firstVertex;
	FVector2D secondVertex;
	float radius;

	FTwoDColliderVertexForm() {}

	FTwoDColliderVertexForm(const FEnumTypeOfHitbox type, const FVector2D center, float YawRotation, float lengthFromCenter, float widthFromCenter) {
		EnumTypeOfHitbox = type;
		float realLength;
		float realWidth;
		float realAngle;
		if (lengthFromCenter > widthFromCenter) {
			realLength = lengthFromCenter;
			realWidth = widthFromCenter;
			realAngle = YawRotation;
		}
		else {
			realLength = widthFromCenter;
			realWidth = lengthFromCenter;
			realAngle = YawRotation + PI / 2;
		}

		float distanceToFoci;
		if (type == Square) {
			radius = realWidth;
			distanceToFoci = realLength - radius;
		}
		else if (type == Ellipse) {
			distanceToFoci = sqrt(pow(realLength, 2) - pow(realWidth, 2));
			radius = realLength - distanceToFoci;
		}

		float xAdjustment = cos(realAngle) * distanceToFoci;
		float yAdjustment = sin(realAngle) * distanceToFoci;
		firstVertex.X = center.X + xAdjustment;
		firstVertex.Y = center.Y + yAdjustment;
		secondVertex.X = center.X - xAdjustment;
		secondVertex.Y = center.Y - yAdjustment;
	}
};

USTRUCT()
struct FTwoDColliderCenterForm {
	GENERATED_BODY()

	TEnumAsByte<FEnumTypeOfHitbox> EnumTypeOfHitbox;
	FVector2D center;
	float YawRotation;
	float lengthFromCenter;
	float widthFromCenter;

	FTwoDColliderCenterForm() {}

	FTwoDColliderCenterForm(const FEnumTypeOfHitbox type, const FVector2D newCenter, float Yaw, float lengthFromCent, float widthFromCent) {
		EnumTypeOfHitbox = type;
		center = newCenter;
		YawRotation = Yaw;
		lengthFromCenter = lengthFromCent;
		widthFromCenter = widthFromCent;
	}

	static float ValueOfTAtPoint(const FTwoDColliderCenterForm ellipse, const FVector2D point, bool bIsSimple) { //a >= b; returns in range -pi/2 to 3/2pi; bIsSimple = true means centered at (0,0) with Yaw 0; WARNING: no implementation for false
		float T;
		if (bIsSimple) {
			float numer = point.Y * ellipse.lengthFromCenter;
			float denom = point.X * ellipse.widthFromCenter;
			if ((denom > 0 && denom < EPSILON) || (denom <= 0 && denom > EPSILON * -1)) {
				if (numer >= 0) {
					T = PI / 2;
				}
				else {
					T = -1 * PI / 2;
				}
			}
			else {
				T = atan(numer / denom);
			}

			if (point.X < 0) {
				T += PI / 2;
			}

		}
		else {
			//not implemented
			return 0.0;
		}
		return T;
	}

	static FTwoDColliderCenterForm RelativeToSystemOfCoordsAtTransform(const FVector2D newOrigin, float newYaw, const FTwoDColliderCenterForm relativeTwoDColliderCenterForm) {
		FTwoDColliderCenterForm result;
		result.center.X = relativeTwoDColliderCenterForm.center.X - newOrigin.X;
		result.center.Y = relativeTwoDColliderCenterForm.center.Y - newOrigin.Y;
		result.YawRotation = relativeTwoDColliderCenterForm.YawRotation - newYaw;
		result.lengthFromCenter = relativeTwoDColliderCenterForm.lengthFromCenter;
		result.widthFromCenter = relativeTwoDColliderCenterForm.widthFromCenter;
	}

	void NormalizeLengthWidth() {
		if (lengthFromCenter < widthFromCenter) {
			widthFromCenter = lengthFromCenter;
			lengthFromCenter = widthFromCenter;
			YawRotation += PI / 2;
		}
	}

	static FTwoPoints FindLinePointsOnEllipse(float a, float b, const FLineEquation line) { //for an non-rotated ellipse at (0,0); a >= b; returns 2 points maximum, if only one secondPoint = null
		FTwoPoints twoPoints;

		//squaring doubles possible roots, fake ones are for the line reflected over X-axis
		float firstCosT; // check positive and negative
		float secondCosT; // check positive and negative

		if (line.bKinfinite) { //if a*cos(t) = line.equationB
			firstCosT = line.equationB / a;
			secondCosT = firstCosT;
		}
		else {
			//note: quadratic equation has been optimized (eliminated multiples of 2 in numerator and denom), so it looks a bit weird
			float quadraticA = pow(b, 2) + pow(line.equationK, 2);
			float quadraticB = line.equationB * line.equationK * a;
			float quadraticC = pow(line.equationB, 2) - pow(b, 2);

			float D = pow(quadraticB, 2) - (quadraticA * quadraticC);
			if (D <= 0 || (quadraticA > 0 && quadraticA < EPSILON) || (quadraticA <= 0 && quadraticA > EPSILON * -1) ) {
				//set cos to impossible values to signify non-existant roots
				firstCosT = 2;
				secondCosT = 2;
			}
			else {
				firstCosT = ((quadraticB * -1) + sqrt(D)) / quadraticA;
				secondCosT = ((quadraticB * -1) - sqrt(D)) / quadraticA; //theoretically, this should always be equal to -firstCosT + PI; 
			}
		}

		if (-1 <= firstCosT && firstCosT <= 1) { //checks if there are roots; theoretically, when this is true (-1 <= secondCosT && secondCosT <= 1) is also true
			float firstT = acos(firstCosT);
			float secondT;
			if (secondCosT > 1) { //to avoid rounding errors
				secondT = 0.f;
			}
			else if (firstCosT < -1) {
				secondT = PI;
			}
			else {
				secondT = acos(secondCosT);
			}

			FVector2D pointAtFirst = FVector2D(a * cos(firstT), b * sin(firstT));
			FVector2D pointAtSecond = FVector2D(a * cos(firstT * -1), b * sin(firstT * -1)); //first reflected over X-axis
			FVector2D pointAtThird = FVector2D(a * cos(secondT), b * sin(secondT));
			FVector2D pointAtForth = FVector2D(a * cos(secondT * -1), b * sin(secondT * -1)); //third reflected over X-axis

			//check up to 4 roots, no more than 2 will be true
			if (line.CheckPointIsOnLine(pointAtFirst)) {
				twoPoints.firstPoint = pointAtFirst;
				twoPoints.bFirstExists = true;
			}
			else if (line.CheckPointIsOnLine(pointAtSecond)) {
				twoPoints.firstPoint = pointAtSecond;
				twoPoints.bFirstExists = true;
			}

			if (line.CheckPointIsOnLine(pointAtThird)) {
				twoPoints.secondPoint = pointAtThird;
				twoPoints.bSecondExists = true;
			}
			else if (line.CheckPointIsOnLine(pointAtForth)) {
				twoPoints.secondPoint = pointAtForth;
				twoPoints.bSecondExists = true;
			}

			//if  they are the same point - remove one
			if (twoPoints.bFirstExists && twoPoints.bSecondExists) {
				float dx = twoPoints.firstPoint.X - twoPoints.secondPoint.X;
				float dy = twoPoints.firstPoint.Y - twoPoints.secondPoint.Y;
				if (
					(dx > 0 && dx < EPSILON) || (dx <= 0 && dx > EPSILON * -1) ||
					(dy > 0 && dy < EPSILON) || (dy <= 0 && dy > EPSILON * -1)
					) {
					twoPoints.bFirstExists = true;
					twoPoints.bSecondExists = false;
				}
			}
		}
		return twoPoints;
	}

	static FTwoPoints FindSegmentPointsOnEllipse(float a, float b, const FLineSegmentPoints segment) {
		FLineEquation line = FLineEquation(segment);
		FTwoPoints points = FindLinePointsOnEllipse(a, b, line);
		if (points.bFirstExists && !segment.IsLinePointInBounds(FVector2D(points.firstPoint.X, points.firstPoint.Y))) {
			points.bFirstExists = false;
		}
		if (points.bSecondExists && !segment.IsLinePointInBounds(FVector2D(points.secondPoint.X, points.secondPoint.Y))) {
			points.bSecondExists = false;
		}
		return points;
	}

	static bool IsEllipseCrossingSegment(float a, float b, const FLineSegmentPoints segment) { //for an non-rotated ellipse at (0,0); a >= b
		FTwoPoints pointsToCheck = FindSegmentPointsOnEllipse(a, b, segment);
		return (pointsToCheck.bFirstExists || pointsToCheck.bSecondExists);
	}

private:
	static float CalculateRateOfChangeForEllipseTwoVertex(float a, float b, float t, const FVector2D firstVertex, const FVector2D secondVertex) { //see EllipseCollisionExplanation.png; a >= b
		float x1 = firstVertex.X;
		float y1 = firstVertex.Y;
		float x2 = secondVertex.X;
		float y2 = secondVertex.Y;
		
		float sint = sin(t);
		float cost = cos(t);

		float numerator1 = ( ( a * cost - x1 ) * ( -1 * sint * a ) + ( b * sint - y1 ) * (cost * b ) );
		float denominator1 = sqrt( pow( ( a * cost - x1 ) , 2) + pow( ( b * sint - y1 ) , 2) );

		float numerator2 = ( ( a * cost - x2 ) * ( -1 * sint * a ) + ( b * sint - y2 ) * (cost * b ) );
		float denominator2 = sqrt( pow( ( a * cost - x2 ) , 2) + pow( ( b * sint - y1 ) , 2) );

		return (numerator1 / denominator1) + (numerator2 / denominator2);
	}

private:
	static float ApproximateTWithMinimalTwoVertexDistance(float a, float b, float firstBoundT, float secondBoundT, const FVector2D firstVertex, const FVector2D secondVertex, unsigned int numOfOperations = 13) { //for an non-rotated ellipse at (0,0); a >= b
		
		//calculate bounding t values
		float valueAtFirst = CalculateRateOfChangeForEllipseTwoVertex(a, b, firstBoundT, firstVertex, secondVertex);
		float valueAtSecond = CalculateRateOfChangeForEllipseTwoVertex(a, b, secondBoundT, firstVertex, secondVertex);
		float lowerBoundT;
		float upperBoundT;

		if ((valueAtFirst >= 0 && valueAtSecond >= 0) || (valueAtFirst < 0 && valueAtSecond < 0)) { //values have to be of different signs
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("ApproximateMinimalTwoVertexDistanceT error: same sign values")));
			return 0.f;
		}

			//determine bounds by having lower bound be the negative derivative one, going counter-clockwise (increasing t) from - to + guarantees min value between 
		if (valueAtFirst < 0) {
			lowerBoundT = firstBoundT;
			upperBoundT = secondBoundT;
		}
		else {
			lowerBoundT = firstBoundT;
			upperBoundT = secondBoundT;
		}

		//make sure upper bound > lower bound, so the algorith just has to pick t in-between them
		if (upperBoundT < lowerBoundT) {
			upperBoundT += 2 * PI;
		}

		//approximation algorithm, moving one or the other bound to midpoint
		//how it should be kept: lower bound derivative negative; upper bound derivative positive
		float valueAtMidpoint;
		for (unsigned int i = 0; i < numOfOperations; i++) {
			float midpointT = (upperBoundT + lowerBoundT) / 2;
			valueAtMidpoint = CalculateRateOfChangeForEllipseTwoVertex(a, b, midpointT, firstVertex, secondVertex);

			if (valueAtMidpoint >= 0) { //if positive -> move positive (upper) bound to midpointT
				upperBoundT = midpointT;
			}
			else { //else negative (lower) bound to midpoint
				lowerBoundT = midpointT;
			}
		}

		return lowerBoundT;
	}

	static bool IsEllipseOverlappingEllipse(FTwoDColliderCenterForm firstEllipseCenterForm, FTwoDColliderCenterForm secondEllipseCenterForm) { //both ellipses, otherwise false
		if (firstEllipseCenterForm.EnumTypeOfHitbox != Ellipse || secondEllipseCenterForm.EnumTypeOfHitbox != Ellipse) {
			return false;
		}

		//Turn both to vector form and check distance between vertices, if less than sum of their radii => they collide
		FTwoDColliderVertexForm firstAsVertex = FTwoDColliderVertexForm(firstEllipseCenterForm.EnumTypeOfHitbox, firstEllipseCenterForm.center, firstEllipseCenterForm.YawRotation, firstEllipseCenterForm.lengthFromCenter, firstEllipseCenterForm.widthFromCenter);
		FTwoDColliderVertexForm secondAsVertex = FTwoDColliderVertexForm(secondEllipseCenterForm.EnumTypeOfHitbox, secondEllipseCenterForm.center, secondEllipseCenterForm.YawRotation, secondEllipseCenterForm.lengthFromCenter, secondEllipseCenterForm.widthFromCenter);
		float sumOfRadii = firstAsVertex.radius + secondAsVertex.radius;
		if (
			FVector2D::Distance(firstAsVertex.firstVertex, secondAsVertex.firstVertex) < sumOfRadii ||
			FVector2D::Distance(firstAsVertex.firstVertex, secondAsVertex.secondVertex) < sumOfRadii ||
			FVector2D::Distance(firstAsVertex.secondVertex, secondAsVertex.firstVertex) < sumOfRadii ||
			FVector2D::Distance(firstAsVertex.secondVertex, secondAsVertex.secondVertex) < sumOfRadii
			) {
			return true;
		}
		//Steps to check ellipse on ellipse collision

		//1.	Take one of them(firstEllipseCenterForm) and set it's tranform as origin for the coordinate system, secondAsRelativeToFirst has coords relative to firstEllipseCenterForm
		FTwoDColliderCenterForm secondAsRelativeToFirst = FTwoDColliderCenterForm::RelativeToSystemOfCoordsAtTransform(firstEllipseCenterForm.center, firstEllipseCenterForm.YawRotation, secondEllipseCenterForm);
		FTwoDColliderVertexForm secondAsRelativeInVertex = FTwoDColliderVertexForm(secondAsRelativeToFirst.EnumTypeOfHitbox, secondAsRelativeToFirst.center, secondAsRelativeToFirst.YawRotation, secondAsRelativeToFirst.lengthFromCenter, secondAsRelativeToFirst.widthFromCenter);
		FVector2D secondVertex1 = secondAsRelativeInVertex.firstVertex;
		FVector2D secondVertex2 = secondAsRelativeInVertex.secondVertex;
		//From this point onward calculations assume firstEllipseCenterForm has its center at (0,0) and Yaw = 0; only length and width for this object matter now!
		//Meanwhile other ellipse is represented by 2 vertices, to which we measure distance

		//2.	Check if firstEllipseCenterForm overlaps segment created from two vertices, if not - proceed
		firstEllipseCenterForm.NormalizeLengthWidth(); //length > width
		secondEllipseCenterForm.NormalizeLengthWidth(); //important!
		if (IsEllipseCrossingSegment(firstEllipseCenterForm.lengthFromCenter, firstEllipseCenterForm.widthFromCenter, FLineSegmentPoints(secondVertex1, secondVertex2))) {
			return true;
		}
		//Now we are sure there is only 1 point that minimizes sum of distances to vertices of secondAsRelativeToFirst

		//3.	Connect both sets of vertices to find boundaries for our approximation (see EllipseCollisionExplanation.png)
		//		They are guaranteed to contain the min point; furthermore, rate of change at those t's is of opposite sign, which is needed for approximation method
		FVector2D firstBasicVertex = FVector2D(firstEllipseCenterForm.lengthFromCenter, 0.f);
		FVector2D secondBasicVertex = FVector2D(firstEllipseCenterForm.lengthFromCenter * -1, 0.f);

		FLineSegmentPoints firstSegment = FLineSegmentPoints(firstBasicVertex, secondVertex1);
		FLineSegmentPoints secondSegment = FLineSegmentPoints(secondBasicVertex, secondVertex2);

		if (FLineEquation::CheckSegmentsCross(firstSegment, secondSegment)) { //we need a configuration of non-crossing segments; resolved by swapping vertices if cross
			firstSegment = FLineSegmentPoints(firstBasicVertex, secondVertex2);
			secondSegment = FLineSegmentPoints(secondBasicVertex, secondVertex1);
		}

		//get overlap with ellipse of firstSegment
		float firstBoundT;
		float secondBoundT;

		FTwoPoints firstOverlap = FindSegmentPointsOnEllipse(firstEllipseCenterForm.lengthFromCenter, firstEllipseCenterForm.widthFromCenter, firstSegment);
		if (firstOverlap.bFirstExists) {
			firstBoundT = ValueOfTAtPoint(firstEllipseCenterForm, firstOverlap.firstPoint, true);
		}
		else if (firstOverlap.bSecondExists) {
			firstBoundT = ValueOfTAtPoint(firstEllipseCenterForm, firstOverlap.secondPoint, true);
		}
		else { //should never fire, as second ellipse vertices are outside the first ellipse, as checked in Step 2.
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Ellipse/Ellipse collision check error: no bound could be selected")));
			return true;
		}

		FTwoPoints secondOverlap = FindSegmentPointsOnEllipse(firstEllipseCenterForm.lengthFromCenter, firstEllipseCenterForm.widthFromCenter, secondSegment);
		if (secondOverlap.bFirstExists) {
			secondBoundT = ValueOfTAtPoint(firstEllipseCenterForm, secondOverlap.firstPoint, true);
		}
		else if (firstOverlap.bSecondExists) {
			secondBoundT = ValueOfTAtPoint(firstEllipseCenterForm, secondOverlap.secondPoint, true);
		}
		else { //should never fire, as second ellipse vertices are outside the first ellipse, as checked in Step 2.
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Ellipse/Ellipse collision check error: no bound could be selected")));
			return true;
		}

		//4.	Find a point on firstEllipseCenterForm that minimizes sum of distances to vertices of secondAsRelativeToFirst (by approximation)
		float TForMinDistance = ApproximateTWithMinimalTwoVertexDistance(firstEllipseCenterForm.lengthFromCenter, firstEllipseCenterForm.widthFromCenter, firstBoundT, secondBoundT, secondVertex1, secondVertex2);
		// a * cos t = X ; b * sin t = Y
		FVector2D pointForMin = FVector2D( firstEllipseCenterForm.lengthFromCenter * cos(TForMinDistance), firstEllipseCenterForm.widthFromCenter * sin(TForMinDistance) );

		//5.	Find distance from pointForMin to vertices and compare
		float distance1 = sqrt( pow( (pointForMin.X - secondVertex1.X) , 2 ) + pow( (pointForMin.Y - secondVertex1.Y) , 2 ) );
		float distance2 = sqrt( pow( (pointForMin.X - secondVertex2.X), 2) + pow((pointForMin.Y - secondVertex2.Y), 2));

		return (distance1 + distance2) < secondEllipseCenterForm.lengthFromCenter * 2;
	}

	static bool IsOverlappingOther(FTwoDColliderCenterForm firstCenterForm, FTwoDColliderCenterForm secondCenterForm) {
		if (firstCenterForm.EnumTypeOfHitbox == Ellipse && secondCenterForm.EnumTypeOfHitbox == Ellipse) {
			return IsEllipseOverlappingEllipse(firstCenterForm, secondCenterForm);
		}
		else if (true) {
			//TODO
		}
		else {

		}
		
		return false;
	}
};