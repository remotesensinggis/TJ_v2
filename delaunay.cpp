#include "delaunay.h"

void Delaunay::drawDelaunay(const Mat& src,Scalar delaunayColor){
    bool draw;
    Mat dst=src.clone();
    if(dst.type()==CV_8UC1){
        cvtColor(dst,dst,CV_GRAY2RGB);
    }
    for(size_t i = 0; i < triangulation.size(); ++i)
    {
        triangle tri = triangulation[i];

        draw=true;
        Point2f pts[3];
        pts[0]=tri.vtx[0].pt;
        pts[1]=tri.vtx[1].pt;
        pts[2]=tri.vtx[2].pt;

        for(int i=0;i<3;i++){
            if((pts[i].x>src.cols-1)||(pts[i].y>src.rows-1)||(pts[i].x<0)||(pts[i].y<0))
                draw=false;
        }

        if (draw){
            cv::line(dst, pts[0], pts[1], delaunayColor, 1);
            cv::line(dst, pts[1], pts[2], delaunayColor, 1);
            cv::line(dst, pts[2], pts[0], delaunayColor, 1);
        }
    }
    showImage(dst,"Delaunay",imagescale);
}

double Delaunay::calTriArea(const Point2f pt1, const Point2f pt2, const Point2f pt3)
{
//    vector< vector<Point2f>> contour;
    vector<Point2f> contour;
    contour.push_back(pt1);
    contour.push_back(pt2);
    contour.push_back(pt3);
//    contour.push_back(tri);
    return contourArea(contour);
}

double Delaunay::calTriArea(const Delaunay::vertex *v)
{
    Point2f p1,p2,p3;
    p1=v[0].pt;
    p2=v[1].pt;
    p3=v[2].pt;
    return calTriArea(p1,p2,p3);
}

void Delaunay::generateDelaunay(const vector<Point2f> &pts,const vector<double>& attribute)
{
    this->insert(pts);
    getTriangulation(attribute);
    std::sort(triangulation.begin(),triangulation.end(),compTri);//descend
}

void Delaunay::generateDelaunay(const vector<KeyPoint> &kpts,const vector<double>& attribute){
    vector<Point2f> pts;
    KeyPoint2Point2f(kpts,pts);
    generateDelaunay(pts,attribute);
}

void Delaunay::generateDelaunay(const vector<Match> &matches)
{
    vector<double> attribute;
    for(int i=0;i<matches.size();++i){
        double attri=matches[i].p1.x-matches[i].p2.x;
        attribute.push_back(attri);
    }
    vector<Point2f> l,r;
    getPtsFromMatches(matches,l,r);
    generateDelaunay(l,attribute);
}

void Delaunay::getTriangulation(const vector<double> &attribute){
    vector<double> attri_backup=attribute;

    if(attribute.size()!=0)
        assert(attribute.size()==(this->vtx.size()-4));
    else
        attri_backup=vector<double>(this->vtx.size()-4,0);

            triangulation.clear();
            int total = (int)(this->qedges.size()*4);
            vector<bool> edgemask(total, false);
            int idx=0;

            for(int i = 4; i < total; i += 2 ){
                if( edgemask[i] )
                    continue;
                Point2f pt[3];
                int pt_id[3];

                int edge = i;

                pt_id[0]=edgeOrg(edge, &pt[0]);
                if (pt_id[0]<4) continue;
                edgeOrg(edge, &pt[0]);
                edgemask[edge] = true;
                edge = getEdge(edge, NEXT_AROUND_LEFT);

                pt_id[1]=edgeOrg(edge, &pt[1]);
                if(pt_id[1]<4) continue;
                edgeOrg(edge, &pt[1]);
                edgemask[edge] = true;
                edge = getEdge(edge, NEXT_AROUND_LEFT);

                pt_id[2]=edgeOrg(edge, &pt[2]);
                if(pt_id[2]<4) continue;
                edgeOrg(edge, &pt[2]);
                edgemask[edge] = true;

                triangle tri;
                tri.id=idx++;
                for(int k=0;k<3;++k){
                    vertex v;
                    v.id=k;
                    v.pt=pt[k];
                    v.attr=attri_backup[pt_id[k]-4];
                    tri.vtx[k]=v;
                }
                tri.area=calTriArea(tri.vtx);

                triangulation.push_back(tri);
            }
}


//void Delaunay::generateDelaunay(const vector<Match> &matches){
//    vector<Point2f> left_pts,right_pts;
//    getPtsFromMatches(matches,left_pts,right_pts);
//    generateDelaunay(left_pts);

//    dualList.clear();

//    for(int i=0;i<triList.size();++i){
//        dualTri dtri;
//        dtri.id=i;
//        Match tmatches[3];
//        int idx=triList[i].pts_id[0];
//        tmatches[0]=Match(vtx[idx].pt,right_pts[idx-4]);//the first 4 vertices are the 4 corners
//        idx=triList[i].pts_id[1];
//        tmatches[1]=Match(vtx[idx].pt,right_pts[idx-4]);
//        idx=triList[i].pts_id[2];
//        tmatches[2]=Match(vtx[idx].pt,right_pts[idx-4]);


//    }
//}

//void Delaunay::generateDelaunay(const vector<Point2f> &pts, const vector<double> attribute){

//}
