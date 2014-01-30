function [pp,xx,corner_count_x]=reorder_corners(p,dx)
  count = size(p,2);
  if(count == 0)
    pp = [];
    xx = [];
    return
  end
  %Origin corner
  center = mean(p,2);
  dif = bsxfun(@minus,p,center);
  dist = sum(dif.^2,1).^0.5;
  vi = (dif(1,:) > 0) | (dif(2,:) > 0);
  dist(:,vi) = 0;
  [~,i] = max(dist);
  p0 = p(:,i);
  pp = p0;
  xx = [0;0;0];
  p(:,i) = [];
  
  %Find corners
  pref = p0;
  prow0 = p0;
  xi=0;
  yi=0;
  while(~isempty(p))
    found = false;
    
    %Find corner along horizontal direction
    if(size(p,2) == 1)
      found = true;
      i = 1;
    else
      dif = bsxfun(@minus,p,pref);
      dist = sum(dif.^2).^0.5;
      [~,si] = sort(dist);

      s1 = si(1);
      s2 = si(2);
      if(dif(1,s1) > dif(2,s1))
        found = true;
        i = s1;
      elseif(dif(1,s2) > dif(2,s2))
        found = true;
        i = s2;
      end
    end
    
    if(found)
      p_i = p(:,i);
      xi = xi+1;
      pp = [pp,p_i];
      xx = [xx, dx*[xi;yi;0]];
      p(:,i) = [];
      pref = p_i;
    else
      %Find corner along vertical direction
      dif = bsxfun(@minus,p,prow0);
      dist = sum(dif.^2).^0.5;
      [~,si] = sort(dist);

      s1 = si(1);
      dir1 = p(:,s1)-prow0;
      dir1 = dir1 / norm(dir1);

      s2 = si(2);
      dir2 = p(:,s2)-prow0;
      dir2 = dir2 / norm(dir2);
      
      angle = acos(dot(dir1,dir2));
      if(angle < pi/6)
        i = s1;
      elseif(dir1(1) < dir2(1))
        i = s1;
      else
        i = s2;
      end
      p_i = p(:,i);

      corner_count_x = xi+1;
      
      xi = 0;
      yi = yi+1;
      pp = [pp,p_i];
      xx = [xx, dx*[xi;yi;0]];
      p(:,i) = [];
      pref = p_i;
      prow0 = p_i;
    end
%     dif_xy = [sum(bsxfun(@times,dif,dir_x)); sum(bsxfun(@times,dif,dir_y))];
%     
%     %Find corner along dir_x
%     vi = (dif_xy(1,:) > 0.5) & (dif_xy(2,:) < 0.5);
%     if(any(vi))
%       dif_xy(1,~vi) = inf;
%       [~,i] = min(dif_xy(1,:));
%       
%       pp = [pp, p(:,i)];
%       xi=xi+1;
%       xx = [xx, dx*[xi;yi;0]];
%       p(:,i) = [];
%     else
%       %Find corner along dir_y
%       vi = (dif_xy(1,:) < 0.5) & (dif_xy(2,:) > 0.5);
%       if(any(vi))
%         dif_xy(2,~vi) = inf;
%         [~,i] = min(dif_xy(2,:));
% 
%         pp = [pp, p(:,i)];
%         yi=yi+1;
%         xi=0;
%         xx = [xx, dx*[xi;yi;0]];
%         pref = p(:,i);
%         p(:,i) = [];
%       else
%         error('There are corners left but the order couldn''t be determined.');
%       end
%     end    
  end
end